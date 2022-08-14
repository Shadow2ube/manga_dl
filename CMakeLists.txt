cmake_minimum_required(VERSION 3.23)
project(manga_dl)


set(CMAKE_CXX_STANDARD 23)
include(cmake/LibFindMacros.cmake)


message(STATUS "looking for threads...")
find_package(Threads REQUIRED)
message(STATUS "Looking for curl...")
find_package(CURL REQUIRED)

if (CURL_FOUND)
    message(STATUS "Found curl version: ${CURL_VERSION_STRING}")
    message(STATUS "Using curl include dirs: ${CURL_INCLUDE_DIRS}")
    message(STATUS "Using curl libraries: ${CURL_LIBRARIES}\n")
    list(APPEND manga_dl_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find curl.")
endif ()

message(STATUS "looking for curlpp...")
libfind_package(cURLpp CURL)
libfind_pkg_check_modules(CURLPP curlpp)
find_path(CURLPP_INCLUDE_DIR NAMES curlpp PATHS ${CURLPP_INCLUDE_DIRS})
find_library(CURLPP_LIBRARY NAMES curlpp PATHS ${CURLPP_LIBRARY_DIRS})
set(CURLPP_PROCESS_INCLUDES CURLPP_INCLUDE_DIR)
set(CURLPP_PROCESS_LIBS CURLPP_LIBRARY)
libfind_process(CURLPP)
libfind_version_header(CURLPP curlpp/cURLpp.hpp LIBCURLPP_VERSION)

if (CURLPP_FOUND)
    message(STATUS "Found curlpp version: ${CURLPP_VERSION}")
    message(STATUS "Using curlpp include dir: ${CURLPP_INCLUDE_DIR}")
    message(STATUS "USING curlpp libraries: ${CURLPP_LIBRARY}\n")
    list(APPEND manga_dl_INCLUDE_DIRS ${CURLPP_INCLUDE_DIR})
else ()
    message(FATAL_ERROR "Could not find curlpp.")
endif ()

include_directories( ${Project_INCLUDE_DIRS})

add_executable(manga_dl
        lib/task_manager.hpp # task manager library https://github.com/shadow2ube/task_manager
        lib/json.hpp
        src/main.cpp src/util.h src/constants.h src/mdlx/mdlx_parse.h src/mdlx/mdlx_command.h src/printers.h src/mdlx/mdlx_types.h src/http.h src/webdriver/webdriver.h src/webdriver/webdriver.cpp)


target_link_libraries(manga_dl INTERFACE Threads::Threads PRIVATE ${CURL_LIBRARIES} ${CURLPP_LIBRARY})
#target_link_libraries(manga_dl INTERFACE Threads::Threads -Iinclude)
