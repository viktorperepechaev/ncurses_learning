#include "WelcomeScreen.h"

void WelcomeScreen::DisplayInTheCenter() const {
  int terminalNumberOfRows;
  int terminalNumberOfColumns;

  getmaxyx(stdscr, terminalNumberOfRows, terminalNumberOfColumns);

  int firstMessageLength = mainWelcomeMessage_.size();
  int secondMessageLength = bottomWelComeMessage_.size();

  mvprintw(terminalNumberOfRows / 2,
           terminalNumberOfColumns / 2 - firstMessageLength / 2,
           mainWelcomeMessage_.c_str());
  mvprintw(terminalNumberOfRows - 1,
           terminalNumberOfColumns / 2 - secondMessageLength / 2,
           bottomWelComeMessage_.c_str());
}