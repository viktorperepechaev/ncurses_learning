#pragma once

#include <ncurses.h>

#include <string>

class WelcomeScreen {
 public:
  WelcomeScreen() = default;
  
  void DisplayInTheCenter() const;

 private:
  const std::string mainWelcomeMessage_ = "The Game Of Life";
  const std::string bottomWelComeMessage_ = "press any key to continue...";
};