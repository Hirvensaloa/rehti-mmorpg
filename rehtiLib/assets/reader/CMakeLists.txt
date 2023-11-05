cmake_minimum_required(VERSION 3.27)
project(rehti_assets_reader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET rehtiAssetsReader)

add_library(${TARGET} STATIC 
  src/ItemReader.hpp 
  src/ItemReader.cpp 
  src/MapReader.hpp 
  src/MapReader.cpp
  src/ObjectReader.hpp 
  src/ObjectReader.cpp 
  src/SkillReader.hpp 
  src/SkillReader.cpp 
  src/AssimpObjectReader.hpp
  src/AssimpObjectReader.cpp
  src/Utils.hpp
  src/Utils.cpp
  ../Config.hpp)

set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/build/third_party/rapidjson/include)

# assimp

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../third_party/assimp ${CMAKE_CURRENT_BINARY_DIR}/build/assimp)
target_link_libraries(${TARGET} PUBLIC assimp::assimp)

# utils

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../utils ${CMAKE_CURRENT_BINARY_DIR}/build/utils)
target_link_libraries(${TARGET} PUBLIC rehtiUtils)
