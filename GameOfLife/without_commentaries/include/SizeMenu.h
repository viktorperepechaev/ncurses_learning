#pragma once

#include <ncurses.h>

#include <string>
#include <vector>

class SizeMenu {
 public:
  SizeMenu(const int height, const int width, const int startY,
           const int startX, const std::vector<std::string>& keys,
           const std::vector<std::string>& values,
           const int maxOptionValueSize);

  std::vector<std::string> ChooseOptionCycle();

 private:
  void MoveUp();
  void MoveDown();

  void Display() const;

  WINDOW* menu_ = nullptr;
  int insideWidth_;
  int maxOptionValueSize_;
  std::vector<std::string> optionKeys_;
  std::vector<std::string> optionValues_;
  size_t numberOfTheHighlightedOption;
};