cmake_minimum_required(VERSION 3.27)

project(rehti_map_loader)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET maploader)

add_executable(${TARGET} main.cpp src/loader.hpp src/utils.hpp lodepng/lodepng.cpp lodepng/lodepng.h)

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/rapidjson/include)

