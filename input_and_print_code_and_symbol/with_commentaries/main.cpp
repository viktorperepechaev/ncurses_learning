#include <ncurses.h>

int main() {
  initscr();  // Инициализируем ncurses окружение
  noecho();  // Подавляем автоматический вывод символов

  int character;  // Объявляем символ, который мы будем считывать
  printw("Input a character (ESC to escape): ");  // (printw ставит курсор в
                                                  // первый символ, идущий после
                                                  // сообщения)
  while ((character = getch()) &&
         character != 27) {  // Считываем символ и проверяем его на esc
    clear();  // очищаем весь экран (это нужно, чтобы вывод с прошлой итерации
              // не был на текущей)
    move(5, 0);  // Устанавливаем курсор чуть ниже начала
    printw("Keycode: %d, the symbol itself is: %c", character,
           character);  // Выводим код символа и сам символ
    move(0, 0);  // Возвращаем курсор в самое начало
    printw("Input a character (ESC to escape): ");
    refresh();  // Выводим всё
  }
  endwin();  // Убираем ncurses окружение
  return 0;
}
