#include "GameGrid.h"

GameGrid::GameGrid(const int cellHeight, const int cellWidth,
                   const int gridHeight, const int gridWidth)
    : cellHeight_(cellHeight),
      cellWidth_(cellWidth),
      gridHeight_(gridHeight),
      gridWidth_(gridWidth) {
  DrawBox();
  refresh();
  InputLivingCells();
}

void GameGrid::InputLivingCells() {
  MEVENT event;

  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);

  printf("\033[?1003h\n");

  int ch;
  while (true) {
    ch = getch();
    if (ch == '\n') {
      break;
    }
    if (getmouse(&event) != OK) {
      continue;
    }
    if (event.bstate & BUTTON1_CLICKED) {
      int y = event.y - 1;  // -1 из-за обводки
      int x = event.x - 1;  // -1 из-за обводки
      int yIndex = y / cellHeight_;
      int xIndex = x / cellWidth_;
      if (!AreValidCoordinates(yIndex, xIndex)) {
        continue;
      }
      if (aliveCells_.find(std::make_pair(yIndex, xIndex)) !=
          aliveCells_.end()) {
        aliveCells_.erase(std::make_pair(yIndex, xIndex));
        DisplayWithoutChangingOutputAttributes(yIndex, xIndex);
      } else {
        aliveCells_.insert(std::make_pair(yIndex, xIndex));
        attron(A_REVERSE);
        DisplayWithoutChangingOutputAttributes(yIndex, xIndex);
        attroff(A_REVERSE);
      }
      refresh();
    }
  }

  printf("\033[?1003l\n");

  mousemask(0, nullptr);
}

void GameGrid::StartInfiniteCycle() {
  while (true) {
    clear();
    DrawBox();
    Display();
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
      break;
    }
    Update();
    refresh();
  }
}

void GameGrid::DrawBox() const {
  int start_y = 0, start_x = 0;
  int height = gridHeight_ * cellHeight_ + 2,
      width = gridWidth_ * cellWidth_ + 2;

  mvhline(start_y, start_x, '-', width);
  mvhline(start_y + height - 1, start_x, '-', width);

  mvvline(start_y, start_x, '|', height);
  mvvline(start_y, start_x + width - 1, '|', height);

  mvaddch(start_y, start_x, '+');
  mvaddch(start_y, start_x + width - 1, '+');
  mvaddch(start_y + height - 1, start_x, '+');
  mvaddch(start_y + height - 1, start_x + width - 1, '+');
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
      mvaddch(i + 1, j + 1, ' ');
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