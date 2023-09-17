cmake_minimum_required(VERSION 3.26.0)
project(Client)
set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Boost 1.83.0 REQUIRED COMPONENTS system)

add_library(rehtiGraphics "")

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/build/graphics)
add_subdirectory(../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/build/network)

add_executable(Client src/client.cpp)

target_link_libraries(Client rehtiGraphics)
target_link_libraries(Client rehtiNetwork)
target_link_libraries(Client Boost::system)

target_include_directories(Client PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(Client PUBLIC ../rehtiLib/graphics/include)
target_include_directories(Client PUBLIC ../rehtiLib/network/include)