#include "SizeMenu.h"

SizeMenu::SizeMenu(const int height, const int width, const int startY,
                   const int startX, const std::vector<std::string>& keys,
                   const std::vector<std::string>& values,
                   const int maxOptionValueSize)
    : menu_(newwin(height + 2, width + 2, startY, startX)),
      optionKeys_(keys),
      optionValues_(values),
      numberOfTheHighlightedOption(0),
      insideWidth_(width),
      maxOptionValueSize_(maxOptionValueSize) {
  keypad(menu_, TRUE);
  box(menu_, 0, 0);
}

std::vector<std::string> SizeMenu::ChooseOptionCycle() {
  int ch;
  Display();
  while (true) {
    ch = wgetch(menu_);
    if (ch == KEY_UP) {
      MoveUp();
    }
    if (ch == KEY_DOWN) {
      MoveDown();
    }
    if (ch == '\n' || ch == KEY_ENTER) {
      break;
    }

    if (ch == KEY_BACKSPACE || ch == 127) {
      if (!optionValues_[numberOfTheHighlightedOption].empty()) {
        optionValues_[numberOfTheHighlightedOption].pop_back();
      }
    }

    if (std::isdigit(ch)) {
      if (optionValues_[numberOfTheHighlightedOption].size() <
          maxOptionValueSize_) {
        optionValues_[numberOfTheHighlightedOption].push_back(ch);
      }
    }

    Display();
  }
  return optionValues_;
}

void SizeMenu::MoveUp() {
  if (numberOfTheHighlightedOption != 0) {
    --numberOfTheHighlightedOption;
  }
}

void SizeMenu::MoveDown() {
  if (numberOfTheHighlightedOption !=
      static_cast<int>(optionKeys_.size()) - 1) {
    ++numberOfTheHighlightedOption;
  }
}

void SizeMenu::Display() const {
  wclear(menu_);
  box(menu_, 0, 0);
  wattron(menu_, A_REVERSE);
  mvwprintw(menu_, 1 + numberOfTheHighlightedOption, 1,
            optionKeys_[numberOfTheHighlightedOption].c_str());
  mvwprintw(menu_, 1 + numberOfTheHighlightedOption,
            1 + optionKeys_[numberOfTheHighlightedOption].size(),
            optionValues_[numberOfTheHighlightedOption].c_str());
  mvwhline(menu_, 1 + numberOfTheHighlightedOption,
           1 + optionKeys_[numberOfTheHighlightedOption].size() +
               optionValues_[numberOfTheHighlightedOption].size(),
           ' ',
           insideWidth_ - (optionKeys_[numberOfTheHighlightedOption].size() +
                           optionValues_[numberOfTheHighlightedOption].size()));
  wattroff(menu_, A_REVERSE);

  for (size_t i = 0; i < numberOfTheHighlightedOption; ++i) {
    mvwprintw(menu_, 1 + i, 1, optionKeys_[i].c_str());
    mvwprintw(menu_, 1 + i, 1 + optionKeys_[i].size(),
              optionValues_[i].c_str());
  }
  for (size_t i = numberOfTheHighlightedOption + 1; i < optionKeys_.size();
       ++i) {
    mvwprintw(menu_, 1 + i, 1, optionKeys_[i].c_str());
    mvwprintw(menu_, 1 + i, 1 + optionKeys_[i].size(),
              optionValues_[i].c_str());
  }

  wrefresh(menu_);
}