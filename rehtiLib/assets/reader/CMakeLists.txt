cmake_minimum_required(VERSION 3.27)
project(rehti_assets_reader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET rehtiAssetsReader)


add_library(${TARGET})

set(SOURCES
      ItemReader.hpp
      MapReader.hpp
      ObjectReader.hpp
      SkillReader.hpp
      Utils.hpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(${TARGET}
	PRIVATE ${SOURCES}
	)
  
set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../utils)
target_include_directories(${TARGET} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/rapidjson/include)


