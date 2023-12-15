
cmake_minimum_required(VERSION 3.27.0)
project(rehti_network)

find_package(Boost 1.83.0 REQUIRED COMPONENTS system)

include_directories(./src)

SET(TARGET rehtiNetwork)

add_library(${TARGET})
target_include_directories(${TARGET} PUBLIC ${Boost_INCLUDE_DIRS})

set(SOURCES
      Connection.cpp
      Connection.hpp
      Message.hpp
	MessageQueue.hpp
      api/MessageApi.cpp
      api/MessageApi.hpp
      api/Types.hpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(${TARGET}
	PUBLIC ${SOURCES}
	)

target_link_libraries(${TARGET} PUBLIC Boost::system)
target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
# rapidjson

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../third_party/rapidjson/include)

# utils

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../utils ${CMAKE_CURRENT_BINARY_DIR}/build/utils)
target_link_libraries(${TARGET} PUBLIC rehtiUtils)
