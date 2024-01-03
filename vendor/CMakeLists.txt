add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE raygui/src raygui/styles)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

set(BUILD_EXAMPLES OFF CACHE BOOL "disable raylib tests")
add_subdirectory(raylib)