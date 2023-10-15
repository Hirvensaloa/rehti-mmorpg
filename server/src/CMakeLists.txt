########################################################################################################################
### SERVER LIBRARY
########################################################################################################################

find_package(Boost 1.83 COMPONENTS system bind REQUIRED)
find_package(Threads REQUIRED)

set(TARGET server)

set(SOURCES
        Main.cpp
        database/Connect.cpp
        Server.cpp
	Server.hpp

        world/GameWorld.cpp
        world/GameWorld.hpp
        world/Coordinates.hpp
        world/Map.cpp
        world/Map.hpp

        action/Action.cpp
        action/Action.hpp
        action/MoveAction.cpp
        action/MoveAction.hpp

        entity/Entity.hpp
        entity/Entity.cpp
        entity/PlayerCharacter.hpp
        entity/PlayerCharacter.cpp
        entity/Goblin.cpp
        entity/Goblin.hpp
        entity/Npc.cpp
        entity/Npc.hpp
        map/Map.cpp
        map/Map.hpp
	)

add_executable(${TARGET}
        ${SOURCES})

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

target_include_directories(${TARGET} PUBLIC ../../rehtiLib/map/reader)

# postgres client

find_package(libpqxx)
target_link_libraries(${TARGET} PRIVATE libpqxx::pqxx)
