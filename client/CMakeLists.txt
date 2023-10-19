cmake_minimum_required(VERSION 3.27.0)
project(Client)
set(CMAKE_CXX_STANDARD 20)

message("Using CMake version: ${CMAKE_VERSION}")

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/build/graphics)
add_subdirectory(../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/build/network)

set(SOURCES
      Main.cpp
      Client.hpp
      Client.cpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE rehtiGraphics)
target_link_libraries(${PROJECT_NAME} PRIVATE rehtiNetwork)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/graphics/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/network/include)