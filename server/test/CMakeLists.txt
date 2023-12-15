cmake_minimum_required(VERSION 3.27)

set(TARGET mmorpg-server-test)

project(${TARGET})

# GoogleTest requires at least C++14
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  ${TARGET}
  units/Pathfinding.cpp
  ../src/world/Utils.hpp
  ../src/world/Utils.cpp
)

target_link_libraries(
  ${TARGET}
  GTest::gtest_main
  gmock
)

include(GoogleTest)
gtest_discover_tests(${TARGET})
