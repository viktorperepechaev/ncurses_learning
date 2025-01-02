#include <ncurses.h>

#include <string>
#include <unordered_map>
#include <vector>

struct Option {
  std::string name = "";
  bool is_highlighted = false;
};

void DisplayMenu(WINDOW*& win, const std::vector<Option>& choices) {
  box(win, 0, 0);

  int y_pos = 1;
  const int x_pos = 1;

  for (const auto& [name, is_highlighted] : choices) {
    if (is_highlighted) {
      wattron(win, A_REVERSE);
    }

    mvwprintw(win, y_pos, x_pos, name.c_str());

    if (is_highlighted) {
      wattroff(win, A_REVERSE);
    }

    ++y_pos;
  }

  wrefresh(win);
}

void CommandManager(int input_char, WINDOW*& menu, int& current_choice,
                    std::vector<Option>& choices) {
  int previous_choice = current_choice;

  if (input_char == KEY_UP || input_char == KEY_DOWN) {
    if (input_char == KEY_UP) {
      --current_choice;
    } else if (input_char == KEY_DOWN) {
      ++current_choice;
    }

    current_choice = std::max(current_choice, 0);
    current_choice =
        std::min(current_choice, static_cast<int>(choices.size()) - 1);

    choices[previous_choice].is_highlighted = false;
    choices[current_choice].is_highlighted = true;

  } else if (input_char == 'w' || input_char == 'a' || input_char == 's' ||
             input_char == 'd') {
    static const std::unordered_map<int, std::pair<int, int>> shifts = {
        {'w', {-1, 0}}, {'a', {0, -1}}, {'s', {1, 0}}, {'d', {0, 1}}};

    int old_y;
    int old_x;

    getbegyx(menu, old_y, old_x);

    auto [delta_y, delta_x] = shifts.at(input_char);

    int new_y = old_y + delta_y;
    int new_x = old_x + delta_x;

    new_y = std::max(2, new_y);
    new_y = std::min(getmaxy(stdscr) - getmaxy(menu) - 1, new_y);
    new_x = std::max(0, new_x);
    new_x = std::min(getmaxx(stdscr) - getmaxx(menu) - 1, new_x);

    int height = getmaxy(menu);
    int width = getmaxx(menu);

    wclear(menu);
    wrefresh(menu);
    delwin(menu);

    menu = newwin(height, width, new_y, new_x);
  }
}

int main() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);

  refresh();

  std::string rules =
      "Use 'w, a, s, d' to move menu. Use top, down arrows to change the "
      "selected option. 'q' to quit'";
  WINDOW* rules_win = newwin(2, getmaxx(stdscr), 0, 0);
  wborder(rules_win, ' ', ' ', ' ', '.', ' ', ' ', ' ', ' ');
  mvwprintw(rules_win, 0, getmaxx(rules_win) / 2 - rules.size() / 2,
            rules.c_str());
  wrefresh(rules_win);

  std::vector<Option> choices = {Option{"first", true}, Option{"second", false},
                                 Option{"third", false}};
  int max_width = 0;
  for (const auto& [name, is_highlighted] : choices) {
    max_width = std::max(max_width, static_cast<int>(name.size()));
  }
  WINDOW* menu = newwin(choices.size() + 2, max_width + 2, 2, 0);
  box(menu, 0, 0);
  DisplayMenu(menu, choices);

  int current_choice = 0;

  while (true) {
    
    int input_char = getch();
    if (input_char == 'q') {
      break;
    }

    CommandManager(input_char, menu, current_choice, choices);
    DisplayMenu(menu, choices);
  }

  delwin(menu);
  endwin();
  return 0;
}
