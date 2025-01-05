cmake_minimum_required(VERSION 3.10)
project(GameOfLife)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories for header files
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/stages.cpp
    src/WelcomeScreen.cpp
    src/SizeMenu.cpp
    src/GameGrid.cpp
)

# Create the executable
add_executable(GameOfLife ${SOURCES})

# Link ncurses library
target_link_libraries(GameOfLife ncurses)