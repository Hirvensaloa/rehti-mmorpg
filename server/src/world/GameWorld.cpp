#include <memory>
#include <optional>

#include "../entity/Bandit.hpp"
#include "../entity/Goblin.hpp"
#include "../object/LootObject.hpp"
#include "../object/ResourceObject.hpp"
#include "../utils/AssetManager.hpp"
#include "GameWorld.hpp"

#include <iostream>

GameWorld::GameWorld() : mapM(Map()){};

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, Coordinates location)
{
    playersM.push_back(std::make_shared<PlayerCharacter>(this, playerName, playerId, location));
}

bool GameWorld::removePlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if ((*it)->getId() == playerId)
        {

            playersM.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<PlayerCharacter>>& GameWorld::getPlayers()
{
    return playersM;
}

std::shared_ptr<PlayerCharacter> GameWorld::getPlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if ((*it)->getId() == playerId)
        {

            return (*it);
        }
    }
    throw std::runtime_error("Player not found");
}

Map& GameWorld::getMap()
{
    return mapM;
}

void GameWorld::addNpc(Npc npc)
{
    npcsM.push_back(std::make_shared<Npc>(npc));
}

std::vector<std::shared_ptr<Npc>>& GameWorld::getNpcs()
{
    return npcsM;
}

std::map<std::string, std::shared_ptr<Object>>& GameWorld::getObjects()
{
    return objectsM;
}

std::map<Coordinates, std::vector<std::shared_ptr<Item>>> GameWorld::getItems()
{
    return itemsM;
}

void GameWorld::addItem(Coordinates location, std::shared_ptr<Item> item)
{
    if (itemsM.contains(location))
    {
        itemsM[location].push_back(std::move(item));
    }
    else
    {

        itemsM[location] = std::vector<std::shared_ptr<Item>>{std::move(item)};
    }
}

void GameWorld::updateGameWorld()
{

    for (auto npc : npcsM)
    {
        npc->update();
    }
    for (auto p : playersM)
    {
        p->update();
    }
}

void GameWorld::initWorld()
{
    AssetManager::loadAssets();
    Map::loadMap();

    // Add objects to the world
    GameObjects objects = AssetManager::getObjects();
    const std::vector<ObjectLocation>& objectLocations = AssetManager::getObjectLocations();
    for (const ObjectLocation& objectLocation : objectLocations)
    {
        const std::optional<reader::ObjectType> optionalType = objects.getObjectType(objectLocation.id);

        if (optionalType == std::nullopt)
        {
            continue;
        }

        const reader::ObjectType type = optionalType.value();
        if (type == reader::ObjectType::GENERAL)
        {
            const GeneralObjectStruct& generalObject = objects.getGeneralObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y};
            Object genObj(generalObject.id, objectLocation.instanceId, generalObject.name, coords, objectLocation.rotation, type);
            objectsM[objectLocation.instanceId] = std::make_shared<Object>(genObj);
        }
        else if (type == reader::ObjectType::LOOT)
        {
            const LootObjectStruct& lootObject = objects.getLootObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y};
            LootObject lootObj(lootObject.id, objectLocation.instanceId, lootObject.name, coords, objectLocation.rotation, lootObject.yieldableItemList, type);
            objectsM[objectLocation.instanceId] = std::make_shared<LootObject>(lootObj);
        }
        else if (type == reader::ObjectType::RESOURCE)
        {
            const ResourceObjectStruct& resourceObject = objects.getResourceObject(objectLocation.id);
            Coordinates coords{objectLocation.x, objectLocation.y};
            ResourceObject resObj(resourceObject.id, objectLocation.instanceId, resourceObject.name, coords, objectLocation.rotation, resourceObject.yieldableItemList, resourceObject.xpPerYield, resourceObject.depleteChance, resourceObject.relatedSkillId, resourceObject.xpRequirement, type);
            objectsM[objectLocation.instanceId] = std::make_shared<ResourceObject>(resObj);
        }
    }
    std::cout << "Objects added to the world" << std::endl;

    npcsM.push_back(std::make_shared<Goblin>(this, "Kimmo-Goblin", 1337, Coordinates(1, 1)));
    npcsM.push_back(std::make_shared<Bandit>(this, "Roisto-Pena", 123, Coordinates(5, 5)));
    npcsM.back()->getInventory().addItem(AssetManager::createItemInstance(1));
    npcsM.back()->getInventory().useItem(1);
    std::cout << "NPCs added to the world" << std::endl;

    std::cout << "Game world initialized" << std::endl;
}

std::shared_ptr<Entity> GameWorld::getEntity(unsigned int entityId)
{
    for (auto npc : npcsM)
    {
        if (npc->getId() == entityId)
        {
            return npc;
        }
    }
    for (auto player : playersM)
    {
        if (player->getId() == entityId)
        {
            return player;
        }
    }
    throw std::runtime_error("Entity not found");
}