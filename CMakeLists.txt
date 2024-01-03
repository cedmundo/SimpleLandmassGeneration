cmake_minimum_required(VERSION 3.26)
project(SimpleLandmassGeneration C)

set(CMAKE_C_STANDARD 23)
add_subdirectory(vendor)

add_executable(SimpleLandmassGeneration main.c)
target_link_libraries(SimpleLandmassGeneration PRIVATE raylib raygui stb)