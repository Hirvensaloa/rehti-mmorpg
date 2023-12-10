#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>

#include "ItemReader.hpp"
#include "SkillReader.hpp"
#include "Utils.hpp"

/**
 * @brief Describes the location of an object on the map. This is used to spawn object instances on the map.
 */
struct ObjectLocation
{
    unsigned id;
    std::string instanceId;
    int x;
    int y;
    int z;
    unsigned rotation;
};

/**
 * @brief Generates a unique id for an object instance.
 *
 * @param objectLocation ObjectLocation
 * @return unique id string
 */
std::string generateObjectInstanceId(ObjectLocation objectLocation);

/**
 * @brief Reads all the objects on the map. Server can use this to spawn objects on the map.
 *
 * @return List of ObjectLocation structs
 */
const std::vector<ObjectLocation> readObjectLocations();

struct GeneralObjectStruct
{
    int id;
    std::string name;
    std::string description;
    std::vector<std::vector<std::string>> tileMap;
    std::string textureFilename;
    std::string objFilename;
};

struct YieldableItem
{
    int itemId;
    int yieldPercentage;
};

struct YieldableItems
{
    std::vector<YieldableItem> yieldableItemList;
    int xpPerYield;
};

struct ItemTransform
{
    int itemId;
    int resultItemId;
    int resultItemQuantity;
    int xpPerTransform;
};

struct ResourceObjectStruct : public GeneralObjectStruct
{
    YieldableItems yieldableItems;
    std::vector<ItemTransform> itemTransformList;
    int depleteChance;
    int relatedSkillId;
    int xpRequirement;
    std::string characterInteractAnimation;
};

struct LootObjectStruct : public GeneralObjectStruct
{
    std::vector<YieldableItem> yieldableItemList;
    std::string characterInteractAnimation;
};

namespace reader
{
    enum class ObjectType
    {
        GENERAL,
        RESOURCE,
        LOOT
    };
}

/**
 * @brief Contains all the objects defined in the objects.json file
 */
struct GameObjects
{
    std::map<int, GeneralObjectStruct> generalObjects;
    std::map<int, ResourceObjectStruct> resourceObjects;
    std::map<int, LootObjectStruct> lootObjects;

    bool containsId(int id)
    {
        return generalObjects.contains(id) || resourceObjects.contains(id) || lootObjects.contains(id);
    }

    // Returns object type for given object id. Call this first and then use the appropriate getter
    const std::optional<reader::ObjectType> getObjectType(int id)
    {
        if (generalObjects.contains(id))
        {
            return reader::ObjectType::GENERAL;
        }
        else if (resourceObjects.contains(id))
        {
            return reader::ObjectType::RESOURCE;
        }
        else if (lootObjects.contains(id))
        {
            return reader::ObjectType::LOOT;
        }
        else
        {
            return std::nullopt;
        }
    }

    // Get object tile map for given object id (returns empty vector if not found)
    std::vector<std::vector<std::string>> getTileMap(int id)
    {
        std::optional<reader::ObjectType> type = getObjectType(id);

        if (type == std::nullopt)
        {
            return {};
        }

        switch (type.value())
        {
        case reader::ObjectType::GENERAL:
            return generalObjects[id].tileMap;
        case reader::ObjectType::RESOURCE:
            return resourceObjects[id].tileMap;
        case reader::ObjectType::LOOT:
            return lootObjects[id].tileMap;
        default:
            return {};
        }
    }

    const GeneralObjectStruct& getGeneralObject(int id)
    {
        return generalObjects.at(id);
    }

    const ResourceObjectStruct& getResourceObject(int id)
    {
        return resourceObjects.at(id);
    }

    const LootObjectStruct& getLootObject(int id)
    {
        return lootObjects.at(id);
    }

    /**
     * @brief Returns all the object ids that exist
     * @return Vector of object ids
     */
    const std::vector<int> getObjectIds() const
    {
        std::vector<int> objectIds;

        for (const auto object : generalObjects)
        {
            objectIds.push_back(object.second.id);
        }
        for (const auto object : resourceObjects)
        {
            objectIds.push_back(object.second.id);
        }
        for (const auto object : lootObjects)
        {
            objectIds.push_back(object.second.id);
        }

        return objectIds;
    }
};

/**
 * @brief Reads objects defined in the objects.json file and returns them as a GameObjects struct
 *
 * @param gameItems GameItems
 * @param gameSkills GameSkills
 * @return GameObjects
 */
GameObjects fetchObjects(GameItems& gameItems, std::map<int, GameSkill>& gameSkills);