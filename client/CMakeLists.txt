cmake_minimum_required(VERSION 3.27.0)
project(Client)
set(CMAKE_CXX_STANDARD 20)

message("Using CMake version: ${CMAKE_VERSION}")


set(SOURCES
      Main.cpp
      Client.hpp
      Client.cpp
      Utils.hpp
      Utils.cpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/build/graphics)
add_subdirectory(../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/build/network)
add_subdirectory(../rehtiLib/assets/reader ${CMAKE_CURRENT_LIST_DIR}/../build/assets/reader)
target_link_libraries(${PROJECT_NAME} PRIVATE rehtiGraphics rehtiNetwork rehtiAssetsReader)
