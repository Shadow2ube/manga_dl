cmake_minimum_required(VERSION 3.23)
project(manga_dl)

set(CMAKE_CXX_STANDARD 23)

find_package(Threads REQUIRED)

add_library(task_manager INTERFACE)
target_include_directories(task_manager INTERFACE lib/)

add_executable(manga_dl
        lib/task_manager.hpp # task manager library https://github.com/shadow2ube/task_manager
        src/main.cpp src/util.h src/constants.h src/mdlx/mdlx_parse.h src/mdlx/mdlx_command.h src/printers.h src/mdlx/mdlx_types.h)

target_link_libraries(manga_dl INTERFACE Threads::Threads task_manager)