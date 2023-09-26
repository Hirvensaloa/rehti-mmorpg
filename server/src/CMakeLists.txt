########################################################################################################################
### SERVER LIBRARY
########################################################################################################################

find_package(Boost 1.83 COMPONENTS system bind REQUIRED)
find_package(Threads REQUIRED)

set(TARGET server)

add_executable(${TARGET}
        Main.cpp database/connect.cpp Server.cpp Server.hpp)

target_compile_features(${TARGET} PUBLIC
        cxx_std_20)

target_include_directories(${TARGET} PUBLIC
        ${Boost_INCLUDE_DIRS})

target_link_libraries(${TARGET} PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads)

add_subdirectory(../../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/../build/network)
target_link_libraries(${TARGET} PUBLIC rehtiNetwork)
target_include_directories(${TARGET} PUBLIC ../../rehtiLib/network/include)

# postgres client

find_package(libpqxx)
target_link_libraries(${TARGET} PRIVATE libpqxx::pqxx)
