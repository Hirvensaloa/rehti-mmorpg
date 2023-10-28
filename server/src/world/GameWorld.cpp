#include <memory>

#include "GameWorld.hpp"
#include "../entity/Bandit.hpp"
#include "../entity/Goblin.hpp"
#include "../object/ResourceObject.hpp"
#include "../object/LootObject.hpp"

#include <iostream>

GameWorld::GameWorld() : mapM(Map()){};

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, Coordinates location)
{
    PlayerCharacter newPlayer = PlayerCharacter(playerName, assetManagerM, playerId, location);
    playersM.push_back(newPlayer);
}

bool GameWorld::removePlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if (it->getId() == playerId)
        {

            playersM.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<PlayerCharacter> &GameWorld::getPlayers()
{
    return playersM;
}

PlayerCharacter *GameWorld::getPlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if (it->getId() == playerId)
        {

            return &(*it);
        }
    }
    return nullptr;
}

Map &GameWorld::getMap()
{
    return mapM;
}

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, Coordinates location)
{
    PlayerCharacter newPlayer = PlayerCharacter(this, playerName, playerId, location);
    playersM.push_back(newPlayer);
}

bool GameWorld::removePlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if (it->getId() == playerId)
        {

            playersM.erase(it);
            return true;
        }
    }
    return false;
}

void GameWorld::addNpc(Npc npc)
{
    npcsM.push_back(std::make_shared<Npc>(npc));
}

std::vector<std::shared_ptr<Npc>> &GameWorld::getNpcs()
{
    return npcsM;
}

std::map<std::string, std::unique_ptr<Object>> &GameWorld::getObjects()
{
    return objectsM;
}

void GameWorld::updateGameWorld()
{
    for (auto npc : npcsM)
    {
        npc->update();
    }
    for (PlayerCharacter &p : playersM)
    {
        p.update();
    }
}

void GameWorld::initWorld()
{
    assetManagerM.loadAssets();

    // Add objects to the world
    GameObjects objects = assetManagerM.getObjects();
    const std::vector<ObjectLocation> &objectLocations = assetManagerM.getObjectLocations();
    for (const ObjectLocation &objectLocation : objectLocations)
    {
        const ObjectType &type = objects.getObjectType(objectLocation.id);

        if (type == ObjectType::GENERAL)
        {
            const GeneralObjectStruct &generalObject = objects.getGeneralObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y, objectLocation.z};
            Object genObj(generalObject.id, objectLocation.instanceId, generalObject.name, coords, objectLocation.rotation);
            objectsM[objectLocation.instanceId] = std::make_unique<Object>(genObj);
        }
        else if (type == ObjectType::LOOT)
        {
            const LootObjectStruct &lootObject = objects.getLootObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y, objectLocation.z};
            LootObject lootObj(lootObject.id, objectLocation.instanceId, lootObject.name, coords, objectLocation.rotation, lootObject.yieldableItemList);
            objectsM[objectLocation.instanceId] = std::make_unique<LootObject>(lootObj);
        }
        else if (type == ObjectType::RESOURCE)
        {
            const ResourceObjectStruct &resourceObject = objects.getResourceObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y, objectLocation.z};
            ResourceObject resObj(resourceObject.id, objectLocation.instanceId, resourceObject.name, coords, objectLocation.rotation, resourceObject.yieldableItemList, resourceObject.xpPerYield, resourceObject.depleteChance, resourceObject.relatedSkillId, resourceObject.xpRequirement);
            objectsM[objectLocation.instanceId] = std::make_unique<ResourceObject>(resObj);
        }
    }

    npcsM.push_back(std::make_shared<Goblin>(this, "Kimmo-Goblin", 1337, Coordinates(1, 1)));
    npcsM.push_back(std::make_shared<Bandit>(this, "Roisto-Pena", 123, Coordinates(5, 5)));
    npcsM.back()->getInventory().addItem(std::make_shared<EquippableItem>(57, "Bronze Scimitar", Slot::MainHand, ItemStats(30, 20, 500, 1)));
    npcsM.back()->getInventory().useItem(57);

    std::cout << "Game world initialized" << std::endl;
}