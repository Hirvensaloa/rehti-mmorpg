########################################################################################################################
### SERVER LIBRARY MAIN
########################################################################################################################

cmake_minimum_required(VERSION 3.26.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### META INFO
########################################################################################################################

set(SERVER_PROJECT_NAME        "server")
set(SERVER_VERSION_MAJOR       "0")
set(SERVER_VERSION_MINOR       "0")
set(SERVER_VERSION_PATCH       "1")

set(SERVER_VERSION
    "${SERVER_VERSION_MAJOR}.${SERVER_VERSION_MINOR}.${SERVER_VERSION_PATCH}")
set(SERVER_NAME_VERSION
    "${SERVER_PROJECT_NAME} v${SERVER_VERSION} (${SERVER_VERSION_REVISION})")

########################################################################################################################
### DECLARE PROJECT
########################################################################################################################

project(${SERVER_PROJECT_NAME})

set(SERVER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)
