cmake_minimum_required(VERSION 3.0)
project(wintree VERSION 0.1.0)

if (NOT WIN32)
	message(FATAL_ERROR "This project is only meaningful when built for Windows.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

find_package(Boost REQUIRED COMPONENT program_options)

add_executable(tree src/main.cpp)
target_compile_options(tree
    PRIVATE
        "/utf-8" # Force UTF-8 source file
)
target_compile_definitions(tree
    PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN=1"
        "WINVER=0x0A00"
        "_WIN32_WINNT=0x0A00"
)
target_link_libraries(tree PRIVATE Boost::program_options)