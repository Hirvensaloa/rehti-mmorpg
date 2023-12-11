#pragma once

#include <map>
#include <optional>
#include <vector>

#include "../entity/Npc.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../object/Object.hpp"
#include "Map.hpp"

class GameWorld
{

public:
    GameWorld();

    ~GameWorld() = default;

    std::vector<std::shared_ptr<PlayerCharacter>>& getPlayers();

    std::shared_ptr<PlayerCharacter> getPlayer(unsigned int playerId);

    Map& getMap();

    /**
     * @brief Adds player to the game world
     *
     * @param playerName
     * @param playerId
     * @param baseDamage
     * @param baseAccuracy
     * @param location Coordinates
     */
    void addPlayer(std::string playerName, unsigned int playerId, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, Coordinates location);

    /**
     * @brief Removes player from the game world
     *
     * @param playerId
     * @return true if successful
     */
    bool removePlayer(unsigned int playerId);

    /**
     * @brief Adds npc to the game world
     *
     * @param npc
     */
    void addNpc(Npc npc);

    /**
     * @brief Get all npcs
     *
     * @return std::vector<std::shared_ptr<Npc>>
     */
    std::vector<std::shared_ptr<Npc>>& getNpcs();

    /**
     * @brief Get npc by id
     *
     * @param npcId
     * @return std::shared_ptr<Npc>
     */
    std::shared_ptr<Npc> getNpc(unsigned int npcId);

    /**
     * @brief Get entity by id
     *
     * @param entityId
     * @return shared pointer to Entity
     */
    std::shared_ptr<Entity> getEntity(unsigned int entityId);

    /**
     * @brief Get all objects
     *
     * @return std::map<std::string, std::shared_ptr<Object>>&
     */
    std::map<std::string, std::shared_ptr<Object>>& getObjects();

    /**
     * @brief Get all items
     *
     * @return std::map<Coordinates, std::vector<std::shared_ptr<Item>>>&
     */
    std::map<Coordinates, std::vector<std::shared_ptr<Item>>> getItems();

    /**
     * @brief Add item to the game world
     *
     * @param location Coordinates where the item is located
     * @param item shared pointer to Item
     */
    void addItem(Coordinates location, std::shared_ptr<Item>);

    /**
     * @brief Updates the game world. Updates all the npcs and players.
     */
    void updateGameWorld();

    /**
     * @brief Initializes the game world. Loads assets and map. Adds npcs and objects to the world.
     */
    void initWorld();

private:
    std::vector<std::shared_ptr<PlayerCharacter>> playersM;

    std::vector<std::shared_ptr<Npc>> npcsM;

    std::map<std::string, std::shared_ptr<Object>> objectsM;

    std::map<Coordinates, std::vector<std::shared_ptr<Item>>> itemsM;

    Map mapM;
};
