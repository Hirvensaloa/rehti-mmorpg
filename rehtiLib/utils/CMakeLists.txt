if (DEFINED Util_GUARD)
  if(NOT Util_GUARD STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    return()
  endif()
else()
  set(Util_GUARD ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "Util_GUARD")
endif()

project(rehti_utils)

set(CMAKE_CXX_STANDARD 20)

SET(TARGET rehtiUtils)

add_library(${TARGET} STATIC src/RapidjsonHelpers.hpp src/RapidjsonHelpers.cpp)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../third_party/rapidjson/include)
