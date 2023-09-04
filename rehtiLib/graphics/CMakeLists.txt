cmake_minimum_required(VERSION 3.10)
project(Graphicsg)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../../dependencies/glfw ./bin)
add_library(Graphics src/base.cpp)
target_link_libraries(Graphics PUBLIC glfw)

target_include_directories(Graphics PUBLIC "./bin" "../../dependencies/glfw/include")