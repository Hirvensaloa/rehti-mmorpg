cmake_minimum_required(VERSION 3.10)
project(Graphics)
set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(../../dependencies/glfw ./bin)
add_library(Graphics "${SOURCE_FILES}/*.h" "${SOURCE_FILES}/*.cpp")
target_link_libraries(Graphics glfw)

target_include_directories(Graphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(Graphics PRIVATE "${YOUR_DIRECTORY}/include")