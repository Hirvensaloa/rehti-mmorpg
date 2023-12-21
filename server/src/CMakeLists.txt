########################################################################################################################
### SERVER LIBRARY
########################################################################################################################

find_package(Boost 1.83 COMPONENTS system bind REQUIRED)
find_package(Threads REQUIRED)

set(TARGET server)

set(SOURCES
        Main.cpp
        database/DatabaseManager.cpp
        database/DatabaseManager.hpp
        Server.cpp
	Server.hpp

        world/GameWorld.cpp
        world/GameWorld.hpp
        world/Coordinates.hpp
        world/Coordinates.cpp
        world/Map.cpp
        world/Map.hpp
        world/Utils.cpp
        world/Utils.hpp

        action/Action.cpp
        action/Action.hpp
        action/AttackAction.cpp
        action/AttackAction.hpp
        action/MoveAction.cpp
        action/MoveAction.hpp
        action/ObjectInteractAction.cpp
        action/ObjectInteractAction.hpp
        action/PickUpAction.hpp
        action/PickUpAction.cpp
        action/RespawnAction.cpp
        action/RespawnAction.hpp

        entity/Entity.hpp
        entity/Entity.cpp
        entity/PlayerCharacter.hpp
        entity/PlayerCharacter.cpp
        entity/PassiveNpc.cpp
        entity/PassiveNpc.hpp
        entity/Npc.cpp
        entity/Npc.hpp
        entity/AggressiveNpc.cpp
        entity/AggressiveNpc.hpp

        item/Equipment.hpp
        item/Equipment.cpp
        item/EquippableItem.hpp
        item/EquippableItem.cpp
        item/Item.hpp
        item/Item.cpp
        item/Inventory.hpp
        item/Inventory.cpp
        item/FoodItem.hpp
        item/FoodItem.cpp

        skill/SkillSet.hpp
        skill/SkillSet.cpp

        utils/AssetManager.hpp
        utils/AssetManager.cpp

        object/LootObject.hpp
        object/LootObject.cpp
        object/Object.hpp
        object/Object.cpp
        object/ResourceObject.hpp
        object/ResourceObject.cpp
	)

add_executable(${TARGET}
        ${SOURCES})

target_compile_features(${TARGET} PRIVATE
        cxx_std_20)

target_include_directories(${TARGET} PRIVATE
        ${Boost_INCLUDE_DIRS})

target_link_libraries(${TARGET} PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads)

add_subdirectory(../../rehtiLib/network ${CMAKE_CURRENT_LIST_DIR}/../build/network)

option(ENABLE_ASSET_EXTENSION "Enable asset extension" OFF)
add_subdirectory(../../rehtiLib/assets/reader ${CMAKE_CURRENT_LIST_DIR}/../build/assets/reader)
target_link_libraries(${TARGET} PRIVATE rehtiAssetsReader PRIVATE rehtiNetwork)

# bcrypt wrapper
add_library(bcrypt ../../third_party/bcrypt/src/bcrypt.cpp ../../third_party/bcrypt/src/blowfish.cpp)
target_include_directories(bcrypt PUBLIC ../../third_party/bcrypt/include)
target_link_libraries(${TARGET} PRIVATE bcrypt)

# postgres client

find_package(libpqxx)
target_link_libraries(${TARGET} PRIVATE libpqxx::pqxx)
