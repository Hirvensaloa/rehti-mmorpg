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
  src/CharacterReader.hpp
  src/CharacterReader.cpp
  src/AnimationReader.hpp
  src/AnimationReader.cpp
  src/Utils.hpp
  src/Utils.cpp
  ../Config.hpp)

set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)

# assimp

# Add an option to enable or disable asset extension (It contains dependencies that are only needed to handle object files, texture files etc. E.g stuff that the server doesn't need). 
option(ENABLE_ASSET_EXTENSION "Enable asset extension" ON)

if (ENABLE_ASSET_EXTENSION)
    find_package(glm REQUIRED)
    # Print message
    message(STATUS "Asset extension enabled")
    target_include_directories(${TARGET} PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/../../../assets/generated)

    # Include Assimp files and all the dependencies
    target_sources(${TARGET} PRIVATE
      src/Assimp.hpp
      src/Assimp.cpp
      src/TextureReader.hpp
      src/TextureReader.cpp
      src/AssetCache.hpp
      src/AssetCache.cpp
      src/AssetTypes.hpp
      stb/stb_image.h      
      )

    target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/includeWithAssimp>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    
    find_package(assimp REQUIRED)
    target_link_libraries(${TARGET} PUBLIC assimp::assimp glm::glm)
else ()
  # Print message
  message(STATUS "Asset extension disabled")
  target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
endif()

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/build/third_party/rapidjson/include)

# utils

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../utils ${CMAKE_CURRENT_BINARY_DIR}/build/utils)
target_link_libraries(${TARGET} PUBLIC rehtiUtils)
