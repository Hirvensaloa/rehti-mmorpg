project(rehtiAudio)

set(CMAKE_CXX_STANDARD 20)

SET(TARGET rehtiAudio)

find_package(SDL2 REQUIRED)

add_library(${TARGET})

set(SOURCES
    RehtiAudio.cpp
    RehtiAudio.hpp
	)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(${TARGET}
	PUBLIC ${SOURCES}
	)

target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../third_party/soloud/contrib ${CMAKE_BINARY_DIR}/build/soloud)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../third_party/soloud/include)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_include_directories(${TARGET} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC ${SDL2_LIBRARIES})

target_link_libraries(${TARGET} PUBLIC soloud)
