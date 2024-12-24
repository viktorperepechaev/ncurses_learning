#include <ncurses.h>

int main() {
  initscr();
  noecho();

  int character;
  printw("Input a character (ESC to escape): ");
  while ((character = getch()) && character != 27) {
    clear();
    move(5, 0);
    printw("Keycode: %d, the symbol itself is: %c", character, character);
    move(0, 0);
    printw("Input a character (ESC to escape): ");
    refresh();
  }
  endwin();
  return 0;
}
