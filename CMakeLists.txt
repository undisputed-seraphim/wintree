cmake_minimum_required(VERSION 3.0)
project(wintree VERSION 0.1.0)

if (NOT WIN32)
	message(FATAL_ERROR "This project only builds in Windows.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tree src/main.cpp)