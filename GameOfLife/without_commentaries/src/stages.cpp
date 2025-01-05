#include "stages.h"

void RunStages() {
  WelcomeScreen().DisplayInTheCenter();

  getch();

  clear();
  refresh();

  std::vector<std::string> options = {"Height: ", "Width: "};
  std::vector<std::string> values = {"10", "10"};

  size_t maxLength =
      std::max_element(options.begin(), options.end(),
                       [](const std::string& a, const std::string& b) {
                         return a.size() < b.size();
                       })
          ->size();

  // 5 - предположительная максимальная длина числа, которое человек введёт для
  // высоты или ширины
  // + 1 - это чтобы не "впритык" было
  std::vector<std::string> userAnswers =
      SizeMenu(options.size(), maxLength + 5 + 1, 0, 0, options, values, 5)
          .ChooseOptionCycle();

  clear();
  refresh();

  std::vector<std::pair<int, int>> aliveCells = {
    {0, 1},
    {1, 2},
    {2, 0},
    {2, 1},
    {2, 2}
  };
  nodelay(stdscr, TRUE);
  GameGrid(1, 2, std::stoi(userAnswers[0]), std::stoi(userAnswers[1]), aliveCells).StartInfiniteCycle();

  getch();
}