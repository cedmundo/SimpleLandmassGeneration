add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE raygui/src raygui/styles)

set(BUILD_EXAMPLES OFF CACHE BOOL "disable raylib tests")
add_subdirectory(raylib)