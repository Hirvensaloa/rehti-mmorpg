cmake_minimum_required(VERSION 3.27)

project(rehti_asset_loader)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET asset_loader)

add_executable(${TARGET} 
  Main.cpp 
  src/MapLoader.hpp 
  src/Utils.hpp 
  lodepng/lodepng.cpp 
  lodepng/lodepng.h
)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../utils)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../reader/src)

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/rapidjson/include)

