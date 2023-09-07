########################################################################################################################
### SERVER LIBRARY
########################################################################################################################

find_package(Boost 1.74 COMPONENTS program_options REQUIRED)
find_package(Threads REQUIRED)

set(TARGET server)

add_executable(${TARGET}
        main.cpp database/connect.cpp)

target_compile_features(${TARGET} PUBLIC
        cxx_std_20)

target_include_directories(${TARGET} PUBLIC
        ${Boost_INCLUDE_DIRS})

target_link_libraries(${TARGET} PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads)

# postgres client

include_directories(/usr/local/include/) 

# find pq library
find_package(libpqxx REQUIRED)
find_library(PQ_LIB pq REQUIRED)

target_link_libraries(${TARGET} PRIVATE
        pqxx
        ${PQ_LIB}
)