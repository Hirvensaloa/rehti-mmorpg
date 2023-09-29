cmake_minimum_required(VERSION 3.27.0)
project(Client)
set(CMAKE_CXX_STANDARD 20)

message("Using CMake version: ${CMAKE_VERSION}")

find_package(glfw3 3.3.8 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(spirv-cross REQUIRED)
find_package(glslang 11.7.0 REQUIRED)
find_package(vulkan-memory-allocator 3.0.1 REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)

add_library(rehtiGraphics "")

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/build/graphics)
add_subdirectory(../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/build/network)

set(SOURCES
      Main.cpp
      Client.cpp
      Client.hpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)
add_executable(Client ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE rehtiGraphics)
target_link_libraries(${PROJECT_NAME} PRIVATE rehtiNetwork)
target_link_libraries(${PROJECT_NAME} PRIVATE GRAPHICS_LIBRARIES)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/graphics/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/network/include)