cmake_minimum_required(VERSION 3.22)
project(manga_dl)

set(CMAKE_CXX_STANDARD 17)

add_executable(manga_dl src/main.cpp src/mangapark.h src/util.h src/constants.h)
