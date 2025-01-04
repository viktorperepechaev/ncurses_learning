#pragma once

#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

struct Cell {
  bool highlighted = false;
  bool wallUp = true;
  bool wallLeft = true;
  bool wallDown = true;
  bool wallRight = true;
};

class Maze {
 public:
  explicit Maze(int rows, int columns, int cell_height, int cell_width)
      : cell_height_(cell_height), cell_width_(cell_width) {
    grid_.resize(rows, std::vector<Cell>(columns));
    graph_.resize(rows, std::vector<std::vector<std::pair<int, int>>>(columns));
    GenerateMaze();
  }

  void DisplayEverythingAtOnce() const {
    DisplayWalls();
    DisplayAngles();
    DisplayAllHighlightedCells();
    refresh();
  }

  void DisplayEverythinAndAnimateSolution() const {
    DisplayWalls();
    DisplayAngles();
    refresh();
    std::vector<std::pair<int, int>> path = Solve();
    for (const auto& [y, x] : path) {
      HighlightCellWithoutRefresh(y, x);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      refresh();
    }
  }

 private:
  std::pair<int, int> GetCoordinatesOfInsideUpLeftCorner(
      int y_index, int x_index) const {  // Нумерация входных номеров с 0
    return std::make_pair(y_index + 1 + cell_height_ * y_index,
                          x_index + 1 + cell_width_ * x_index);
  }

  std::pair<int, int> GetCoordinatesOfInsideDownRightCorner(
      int y_index, int x_index) const {  // Нумерация входных номеров с 0
    return {GetCoordinatesOfInsideUpLeftCorner(y_index, x_index).first +
                cell_height_ - 1,
            GetCoordinatesOfInsideUpLeftCorner(y_index, x_index).second +
                cell_width_ - 1};
  }

  void DisplayWalls() const {
    for (size_t i = 0; i < grid_.size(); ++i) {
      for (size_t j = 0; j < grid_[0].size(); ++j) {
        BuildWallsAroundCellWithoutRefresh(i, j);
      }
    }
  }

  void DisplayAngles() const {
    for (size_t i = 0; i < grid_.size(); ++i) {
      for (size_t j = 0; j < grid_[0].size(); ++j) {
        BuildAnglesAroundCellWithoutRefresh(i, j);
      }
    }
  }

  void DisplayAllHighlightedCells() const {
    for (size_t i = 0; i < grid_.size(); ++i) {
      for (size_t j = 0; j < grid_[0].size(); ++j) {
        if (grid_[i][j].highlighted) {
          HighlightCellWithoutRefresh(i, j);
        }
      }
    }
  }

  void DisplayWallsAndAngles() const {
    DisplayWalls();
    DisplayAngles();
  }

  void HighlightCellWithoutRefresh(const int y_coordinate,
                                   const int x_coordinate) const {
    auto [i_begin, j_begin] =
        GetCoordinatesOfInsideUpLeftCorner(y_coordinate, x_coordinate);
    auto [i_end, j_end] =
        GetCoordinatesOfInsideDownRightCorner(y_coordinate, x_coordinate);
    for (size_t qi = i_begin; qi <= i_end; ++qi) {
      for (size_t qj = j_begin; qj <= j_end; ++qj) {
        attron(A_REVERSE);
        mvaddch(qi, qj, ' ');
        attroff(A_REVERSE);
      }
    }
  }

  chtype GetBestAngle(int y, int x) const {
    unsigned int up_wall = (1 << 0);
    unsigned int right_wall = (1 << 1);
    unsigned int down_wall = (1 << 2);
    unsigned int left_wall = (1 << 3);
    std::unordered_map<unsigned int, chtype> mask_to_character = {
        {up_wall, ACS_BULLET},
        {right_wall, ACS_BULLET},
        {down_wall, ACS_BULLET},
        {left_wall, ACS_BULLET},
        {up_wall | right_wall, ACS_LLCORNER},
        {right_wall | down_wall, ACS_ULCORNER},
        {down_wall | left_wall, ACS_URCORNER},
        {left_wall | up_wall, ACS_LRCORNER},
        {up_wall | down_wall, ACS_VLINE},
        {left_wall | right_wall, ACS_HLINE},
        {up_wall | right_wall | down_wall, ACS_LTEE},
        {right_wall | down_wall | left_wall, ACS_TTEE},
        {down_wall | left_wall | up_wall, ACS_RTEE},
        {left_wall | up_wall | right_wall, ACS_BTEE},
        {up_wall | right_wall | down_wall | left_wall, ACS_PLUS}};

    auto hasVLine = [&](int ry, int rx) {
      int old_y;
      int old_x;
      getyx(stdscr, old_y, old_x);
      chtype ch = mvinch(ry, rx);
      move(old_y, old_x);
      return ch == ACS_VLINE;
    };
    auto hasHLine = [&](int ry, int rx) {
      int old_y;
      int old_x;
      getyx(stdscr, old_y, old_x);
      chtype ch = mvinch(ry, rx);
      move(old_y, old_x);
      return ch == ACS_HLINE;
    };

    unsigned int situation = 0;
    if (y - 1 >= 0 && hasVLine(y - 1, x)) {
      situation |= up_wall;
    }
    if (hasHLine(y, x + 1)) {
      situation |= right_wall;
    }
    if (hasVLine(y + 1, x)) {
      situation |= down_wall;
    }
    if (hasHLine(y, x - 1)) {
      situation |= left_wall;
    }
    return mask_to_character[situation];
  }

  void BuildWallsAroundCellWithoutRefresh(const int y_coordinate,
                                          const int x_coordinate) const {
    auto [i_begin, j_begin] =
        GetCoordinatesOfInsideUpLeftCorner(y_coordinate, x_coordinate);
    auto [i_end, j_end] =
        GetCoordinatesOfInsideDownRightCorner(y_coordinate, x_coordinate);
    // Рисуем стороны
    if (grid_[y_coordinate][x_coordinate].wallUp) {
      mvhline(i_begin - 1, j_begin, ACS_HLINE, cell_width_);
    }
    if (grid_[y_coordinate][x_coordinate].wallDown) {
      mvhline(i_end + 1, j_begin, ACS_HLINE, cell_width_);
    }
    if (grid_[y_coordinate][x_coordinate].wallLeft) {
      mvvline(i_begin, j_begin - 1, ACS_VLINE, cell_height_);
    }
    if (grid_[y_coordinate][x_coordinate].wallRight) {
      mvvline(i_begin, j_end + 1, ACS_VLINE, cell_height_);
    }
  }

  void BuildAnglesAroundCellWithoutRefresh(const int y_coordinate,
                                           const int x_coordinate) const {
    auto [i_begin, j_begin] =
        GetCoordinatesOfInsideUpLeftCorner(y_coordinate, x_coordinate);
    auto [i_end, j_end] =
        GetCoordinatesOfInsideDownRightCorner(y_coordinate, x_coordinate);
    mvaddch(i_begin - 1, j_begin - 1, GetBestAngle(i_begin - 1, j_begin - 1));
    mvaddch(i_begin - 1, j_end + 1, GetBestAngle(i_begin - 1, j_end + 1));
    mvaddch(i_end + 1, j_end + 1, GetBestAngle(i_end + 1, j_end + 1));
    mvaddch(i_end + 1, j_begin - 1, GetBestAngle(i_end + 1, j_begin - 1));
  }

  bool AreCorrectCellCoordinates(int y_coordinate, int x_coordinate) {
    return y_coordinate >= 0 && y_coordinate < grid_.size() &&
           x_coordinate >= 0 && x_coordinate < grid_[0].size();
  }

  void dfsForPath(
      bool& found, int v_y, int v_x, std::vector<std::vector<bool>>& visited,
      std::vector<std::vector<std::pair<int, int>>>& prev) const {
    if (v_y == grid_.size() - 1 && v_x == grid_[0].size() - 1) {
      found = true;
      return;
    }
    visited[v_y][v_x] = true;
    for (auto [next_v_y, next_v_x] : graph_[v_y][v_x]) {
      if (!visited[next_v_y][next_v_x]) {
        prev[next_v_y][next_v_x] = {v_y, v_x};
        dfsForPath(found, next_v_y, next_v_x, visited, prev);
        if (found) {
          return;
        }
      }
    }
  }

  std::vector<std::pair<int, int>> Solve() const {
    // Используем DFS для нахождения пути
    std::vector<std::vector<std::pair<int, int>>> prev(
        grid_.size(), std::vector<std::pair<int, int>>(grid_[0].size()));
    std::vector<std::vector<bool>> visited(
        grid_.size(), std::vector<bool>(grid_[0].size(), false));
    bool found = false;

    dfsForPath(found, 0, 0, visited, prev);

    std::vector<std::pair<int, int>> correct_path;

    int at_y = grid_.size() - 1;
    int at_x = grid_[0].size() - 1;

    while (!(at_y == 0 && at_x == 0)) {
      correct_path.emplace_back(at_y, at_x);

      int new_y = prev[at_y][at_x].first;
      int new_x = prev[at_y][at_x].second;

      at_y = new_y;
      at_x = new_x;
    }

    correct_path.emplace_back(0, 0);

    std::reverse(correct_path.begin(), correct_path.end());

    return correct_path;
  }

  void CarveDfs(int v_y, int v_x, std::vector<std::vector<bool>>& visited) {
    visited[v_y][v_x] = true;

    static const std::vector<std::pair<int, int>> shifts = {
        {-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    std::vector<std::pair<int, int>> possible_shifts;

    for (auto [delta_y, delta_x] : shifts) {
      if (AreCorrectCellCoordinates(v_y + delta_y, v_x + delta_x)) {
        possible_shifts.emplace_back(delta_y, delta_x);
      }
    }

    if (possible_shifts.empty()) {
      return;
    }

    static std::mt19937 rnd(228);
    std::shuffle(possible_shifts.begin(), possible_shifts.end(), rnd);

    for (auto [delta_y, delta_x] : possible_shifts) {
      if (visited[v_y + delta_y][v_x + delta_x]) {
        continue;
      }

      if (delta_y == -1 && delta_x == 0) {
        grid_[v_y][v_x].wallUp = false;
        grid_[v_y + delta_y][v_x + delta_x].wallDown = false;
      }
      if (delta_y == 0 && delta_x == 1) {
        grid_[v_y][v_x].wallRight = false;
        grid_[v_y + delta_y][v_x + delta_x].wallLeft = false;
      }
      if (delta_y == 1 && delta_x == 0) {
        grid_[v_y][v_x].wallDown = false;
        grid_[v_y + delta_y][v_x + delta_x].wallUp = false;
      }
      if (delta_y == 0 && delta_x == -1) {
        grid_[v_y][v_x].wallLeft = false;
        grid_[v_y + delta_y][v_x + delta_x].wallRight = false;
      }

      graph_[v_y][v_x].emplace_back(v_y + delta_y, v_x + delta_x);
      graph_[v_y + delta_y][v_x + delta_x].emplace_back(v_y, v_x);
      CarveDfs(v_y + delta_y, v_x + delta_x, visited);
    }
  }

  void GenerateMaze() {
    std::vector<std::vector<bool>> visited(
        grid_.size(), std::vector<bool>(grid_[0].size(), false));
    CarveDfs(0, 0, visited);
    grid_[0][0].wallLeft = false;
    grid_[grid_.size() - 1][grid_[0].size() - 1].wallRight = false;
    std::vector<std::pair<int, int>> correct_path = Solve();
    for (const auto& [y, x] : correct_path) {
      grid_[y][x].highlighted = true;
    }
  }

  std::vector<std::vector<Cell>> grid_;
  std::vector<std::vector<std::vector<std::pair<int, int>>>> graph_;
  int cell_height_;
  int cell_width_;
};