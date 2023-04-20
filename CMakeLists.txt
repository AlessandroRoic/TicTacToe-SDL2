cmake_minimum_required(VERSION 3.24)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/Debug)

add_executable(TicTacToe main.cpp)

target_link_libraries(TicTacToe PRIVATE SDL2::SDL2)
target_link_libraries(TicTacToe PRIVATE SDL2_image::SDL2_image)
target_link_libraries(TicTacToe PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(TicTacToe PRIVATE SDL2_mixer::SDL2_mixer)
target_link_libraries(TicTacToe PRIVATE fmt::fmt)
