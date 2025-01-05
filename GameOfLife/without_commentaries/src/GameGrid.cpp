#include "GameGrid.h"

GameGrid::GameGrid(const int cellHeight, const int cellWidth,
                   const int gridHeight, const int gridWidth,
                   const std::vector<std::pair<int, int>>& aliveCells)
    : cellHeight_(cellHeight),
      cellWidth_(cellWidth),
      gridHeight_(gridHeight),
      gridWidth_(gridWidth),
      aliveCells_(aliveCells.begin(), aliveCells.end()) {}

void GameGrid::StartInfiniteCycle() {
  while (true) {
    clear();
    Display();
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
      break;
    }
    Update();
  }
}

std::pair<int, int> GameGrid::GetTopLeftCornerCoordinates(int yIndex,
                                                          int xIndex) const {
  return std::make_pair(yIndex * cellHeight_, xIndex * cellWidth_);
}

std::pair<int, int> GameGrid::GetBottomRightCornerCoordinates(
    int yIndex, int xIndex) const {
  auto [y, x] = GetTopLeftCornerCoordinates(yIndex, xIndex);
  return std::make_pair(y + cellHeight_ - 1, x + cellWidth_ - 1);
}

bool GameGrid::AreValidCoordinates(int yCoordinate, int xCoordinate) const {
  return 0 <= yCoordinate && yCoordinate < gridHeight_ && 0 <= xCoordinate &&
         xCoordinate < gridWidth_;
}

void GameGrid::Update() {
  std::set<std::pair<int, int>> newAliveCells;
  std::map<std::pair<int, int>, int> countAliveNear;
  for (const auto& [y, x] : aliveCells_) {
    static const std::vector<std::pair<int, int>> shifts = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
    for (const auto& [deltaY, deltaX] : shifts) {
      int newY = y + deltaY;
      int newX = x + deltaX;
      if (AreValidCoordinates(newY, newX)) {
        ++countAliveNear[std::make_pair(newY, newX)];
      }
    }
  }
  for (const auto& [key, value] : countAliveNear) {
    if ((aliveCells_.find(key) != aliveCells_.end() &&
         (value == 2 || value == 3)) ||
        (aliveCells_.find(key) == aliveCells_.end() && value == 3)) {
      newAliveCells.insert(key);
    }
  }
  aliveCells_ = newAliveCells;
}

void GameGrid::DisplayWithoutChangingOutputAttributes(int y, int x) const {
  auto [startY, startX] = GetTopLeftCornerCoordinates(y, x);
  auto [endY, endX] = GetBottomRightCornerCoordinates(y, x);
  for (int i = startY; i <= endY; ++i) {
    for (int j = startX; j <= endX; ++j) {
      mvaddch(i, j, ' ');
    }
  }
}

void GameGrid::Display() const {
  attron(A_REVERSE);
  for (const auto& [y, x] : aliveCells_) {
    DisplayWithoutChangingOutputAttributes(y, x);
  }
  attroff(A_REVERSE);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}