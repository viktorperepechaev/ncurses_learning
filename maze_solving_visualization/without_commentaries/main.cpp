#include "Maze.hpp"

const int kDesiredHeightOfMaze = 20;
const int kDesiredWidthOfMaze = 60;
const int kDesiredCellHeight = 1;
const int kDesiredCellWidth = 2;

int main() {
  initscr();
  refresh();
  curs_set(0);
  Maze maze(kDesiredHeightOfMaze, kDesiredWidthOfMaze, kDesiredCellHeight,
            kDesiredCellWidth);
  maze.DisplayEverythinAndAnimateSolution();
  refresh();
  getch();
  endwin();
  return 0;
}
