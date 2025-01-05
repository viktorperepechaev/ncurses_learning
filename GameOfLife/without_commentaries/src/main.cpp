#include "stages.h"

int main() {
  initscr();
  curs_set(0);
  noecho();
  refresh();

  RunStages();

  endwin();
  // Нужно узнать:
  //  1) Высоту поля
  //  2) Ширину поля
  //  3) Изначальное расположение живых клеток

  return 0;
}