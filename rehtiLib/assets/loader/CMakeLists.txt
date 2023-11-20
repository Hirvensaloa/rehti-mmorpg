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

option(ENABLE_ASSET_EXTENSION "Enable asset extension" OFF)
add_subdirectory(../reader ${CMAKE_CURRENT_LIST_DIR}/build/reader)
target_link_libraries(${TARGET} rehtiAssetsReader)

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/rapidjson/include)

