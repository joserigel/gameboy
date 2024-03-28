cmake_minimum_required(VERSION 3.2)

project(gameboy)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE GAMEBOY_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(gameboy ${GAMEBOY_SOURCES})