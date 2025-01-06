#include "stages.h"

int main() {
  initscr();
  curs_set(0);
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  refresh();

  RunStages();

  endwin();
  return 0;
}