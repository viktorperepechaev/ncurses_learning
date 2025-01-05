#pragma once

#include <ncurses.h>

#include <map>
#include <set>
#include <vector>
#include <thread>
#include <chrono>

class GameGrid {
 public:
  GameGrid(const int cellHeight, const int cellWidth, const int gridHeight,
           const int gridWidth,
           const std::vector<std::pair<int, int>>& aliveCells);

  void StartInfiniteCycle();

 private:
  std::pair<int, int> GetTopLeftCornerCoordinates(int yIndex, int xIndex) const;
  std::pair<int, int> GetBottomRightCornerCoordinates(int yIndex,
                                                      int xIndex) const;

  bool AreValidCoordinates(int yCoordinate, int xCoordinate) const;

  void Update();

  void DisplayWithoutChangingOutputAttributes(int y, int x) const;
  void Display() const;

  int cellHeight_;
  int cellWidth_;
  int gridHeight_;
  int gridWidth_;
  std::set<std::pair<int, int>> aliveCells_;
};