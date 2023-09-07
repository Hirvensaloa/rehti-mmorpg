cmake_minimum_required(VERSION 3.13)
project(Client)
set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_library(rehtiGraphics "")

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/out)

add_executable(Client src/client.cpp)

target_link_libraries(Client rehtiGraphics)

target_include_directories(Client PUBLIC ../rehtiLib/graphics/include)