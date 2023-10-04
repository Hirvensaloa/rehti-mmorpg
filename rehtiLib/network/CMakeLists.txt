
cmake_minimum_required(VERSION 3.26.0)
project(testi)

find_package(Boost 1.83.0 REQUIRED COMPONENTS system)

include_directories(./src)

add_library(rehtiNetwork)

set(SOURCES
      Connection.cpp
      Connection.hpp
      Message.hpp
	MessageQueue.hpp
      MessageIds.hpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(rehtiNetwork
	PRIVATE ${SOURCES}
	)

target_link_libraries(rehtiNetwork PRIVATE Boost::system)
target_include_directories(rehtiNetwork PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(rehtiNetwork INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
