#include <memory>
#include <optional>

#include "../entity/AggressiveNpc.hpp"
#include "../entity/PassiveNpc.hpp"
#include "../object/LootObject.hpp"
#include "../object/ResourceObject.hpp"
#include "../utils/AssetManager.hpp"
#include "GameWorld.hpp"
#include "Utils.hpp"

#include <iostream>

GameWorld::GameWorld() : mapM(Map()){};

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, Coordinates location)
{
    playersM.push_back(std::make_shared<PlayerCharacter>(this, playerName, baseDamage, baseAccuracy, spawnCoordinateBounds, playerId, location));
}

bool GameWorld::removePlayer(unsigned int playerId)
{
    std::unique_lock<std::mutex> lck(playersMutexM);
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

std::shared_ptr<Npc> GameWorld::getNpc(unsigned int npcId)
{
    for (auto it = npcsM.begin(); it != npcsM.end(); it++)
    {
        if ((*it)->getInstanceId() == npcId)
        {

            return (*it);
        }
    }
    throw std::runtime_error("NPC not found");
}

std::map<std::string, std::shared_ptr<Object>>& GameWorld::getObjects()
{
    return objectsM;
}

std::map<Coordinates, std::vector<std::shared_ptr<Item>>>& GameWorld::getItems()
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

std::shared_ptr<Item> GameWorld::removeItem(Coordinates location, int itemId)
{
    std::unique_lock<std::mutex> lck(itemsMutexM);
    if (itemsM.contains(location))
    {
        for (auto it = itemsM[location].begin(); it != itemsM[location].end(); it++)
        {
            if ((*it)->getInstanceId() == itemId)
            {
                std::shared_ptr<Item> removedItem = std::move(*it);
                itemsM[location].erase(it);
                return removedItem;
            }
        }
    }
    return std::shared_ptr<Item>();
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
            ResourceObject resObj(resourceObject.id, objectLocation.instanceId, resourceObject.name, coords, objectLocation.rotation, resourceObject.yieldableItems.yieldableItemList, resourceObject.yieldableItems.xpPerYield, resourceObject.depleteChance, resourceObject.relatedSkillId, resourceObject.xpRequirement, type, resourceObject.itemTransformList);
            objectsM[objectLocation.instanceId] = std::make_shared<ResourceObject>(resObj);
        }
    }
    std::cout << "Objects added to the world" << std::endl;

    // Add NPCs to the world
    const auto& npcs = AssetManager::getGameCharacters().npcs;
    for (const auto& npc : npcs)
    {
        int i = 0;
        while (i < npc.spawnAmount)
        {
            Coordinates coords = Map::getRandomCoordinates(npc.spawnCoordinateBounds);

            if (npc.agressionType == AggressionType.Peaceful)
            {
                npcsM.push_back(std::make_shared<Npc>(this, npc.name, npc.baseDamage, npc.baseAccuracy, npc.spawnCoordinateBounds, npc.chatResponses, npc.id, coords));
            }
            else if (npc.agressionType == AggressionType.Aggressive)
            {
                npcsM.push_back(std::make_shared<AggressiveNpc>(this, npc.name, npc.agressionRange, npc.baseDamage, npc.baseAccuracy, npc.spawnCoordinateBounds, npc.chatResponses, npc.id, coords));
            }
            else if (npc.agressionType == AggressionType.Passive)
            {
                npcsM.push_back(std::make_shared<PassiveNpc>(this, npc.name, npc.baseDamage, npc.baseAccuracy, npc.spawnCoordinateBounds, npc.chatResponses, npc.id, coords));
            }

            std::cout << "NPC " << npc.name << " added to the world at " << coords.x << " " << coords.y << " " << coords.z << std::endl;
            i++;
        }
    }
    std::cout << "NPCs added to the world" << std::endl;

    std::cout << "Game world initialized" << std::endl;
}

std::shared_ptr<Entity> GameWorld::getEntity(unsigned int entityId)
{
    for (auto npc : npcsM)
    {
        if (npc->getInstanceId() == entityId)
        {
            return npc;
        }
    }
    for (auto player : playersM)
    {
        if (player->getInstanceId() == entityId)
        {
            return player;
        }
    }
    throw std::runtime_error("Entity not found");
}

std::mutex& GameWorld::getPlayersMutex()
{
    return playersMutexM;
}

std::mutex& GameWorld::getItemsMutex()
{
    return itemsMutexM;
}