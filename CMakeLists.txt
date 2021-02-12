cmake_minimum_required(VERSION 3.15)
project(hangman)

set(CMAKE_CXX_STANDARD 20)
add_executable(hangman hangman.cpp)
if(CMAKE_COMPILER_IS_GNU_CC OR CMAKE_COMPILER_IS_GNUCXX)
target_compile_options(hangman PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif(CMAKE_COMPILER_IS_GNU_CC OR CMAKE_COMPILER_IS_GNUCXX)
