cmake_minimum_required(VERSION 3.27.0)
project(Client)
set(CMAKE_CXX_STANDARD 20)

#find_package(Boost 1.83.0 REQUIRED COMPONENTS system)

message("Using CMake version: ${CMAKE_VERSION}")

add_library(rehtiGraphics "")

add_subdirectory(../rehtiLib/graphics ${CMAKE_CURRENT_LIST_DIR}/graphics)
add_subdirectory(../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/network)

add_executable(${PROJECT_NAME} src/client.cpp)

target_link_libraries(${PROJECT_NAME} rehtiGraphics)
target_link_libraries(${PROJECT_NAME} rehtiNetwork)
#target_link_libraries(${PROJECT_NAME} Boost::system)

#target_include_directories(Client PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/graphics/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../rehtiLib/network/include)