#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "ItemReader.hpp"
#include "SkillReader.hpp"
#include "RapidjsonHelpers.hpp"
#include "Utils.hpp"

// Path(s)
const std::string OBJECT_JSON_PATH = ROOT_PATH + "assets/objects.json";
const std::string OBJECTS_JSON_PATH = GENERATED_ASSETS_PATH + "objects.json";

struct ObjectLocation
{
  unsigned id;
  unsigned x;
  unsigned y;
  int rotation;
};

// Reads all the objects on the map. Server can use this to spawn objects on the map.
const std::vector<ObjectLocation> readObjectLocations()
{
  rapidjson::Document objectsJson = readJson(OBJECTS_JSON_PATH);

  std::vector<ObjectLocation> objects;

  for (const auto &object : objectsJson.GetArray())
  {
    ObjectLocation objectLocation;
    objectLocation.id = object["id"].GetUint();
    objectLocation.x = object["x"].GetUint();
    objectLocation.y = object["y"].GetUint();
    objectLocation.rotation = object["rotation"].GetInt();

    objects.push_back(objectLocation);
  }

  return objects;
}

struct GeneralObject
{
  int id;
  std::string name;
  std::string description;
  std::vector<std::vector<std::string>> tileMap;
};

struct YieldableItem
{
  int itemId;
  int yieldPercentage;
};

struct ResourceObject
{
  int id;
  std::string name;
  std::vector<YieldableItem> yieldableItemList;
  int xpPerYield;
  int depleteChance;
  int relatedSkillId;
  int xpRequirement;
  std::string description;
  std::vector<std::vector<std::string>> tileMap;
};

struct LootObject
{
  int id;
  std::string name;
  std::vector<YieldableItem> yieldableItemList;
  std::string description;
  std::vector<std::vector<std::string>> tileMap;
};

enum class ObjectType
{
  GENERAL,
  RESOURCE,
  LOOT,
  NOT_FOUND
};

struct GameObjects
{
  std::map<int, GeneralObject> generalObjects;
  std::map<int, ResourceObject> resourceObjects;
  std::map<int, LootObject> lootObjects;

  bool containsId(int id)
  {
    return generalObjects.contains(id) || resourceObjects.contains(id) || lootObjects.contains(id);
  }

  // Returns object type for given object id. Call this first and then use the appropriate getter
  ObjectType getObjectType(int id)
  {
    if (generalObjects.contains(id))
    {
      return ObjectType::GENERAL;
    }
    else if (resourceObjects.contains(id))
    {
      return ObjectType::RESOURCE;
    }
    else if (lootObjects.contains(id))
    {
      return ObjectType::LOOT;
    }
    else
    {
      return ObjectType::NOT_FOUND;
    }
  }

  // Get object tile map for given object id (returns empty vector if not found)
  std::vector<std::vector<std::string>> getTileMap(int id)
  {
    switch (getObjectType(id))
    {
    case ObjectType::GENERAL:
      return generalObjects[id].tileMap;
    case ObjectType::RESOURCE:
      return resourceObjects[id].tileMap;
    case ObjectType::LOOT:
      return lootObjects[id].tileMap;
    default:
      return {};
    }
  }

  GeneralObject getGeneralObject(int id)
  {
    return generalObjects[id];
  }

  ResourceObject getResourceObject(int id)
  {
    return resourceObjects[id];
  }

  LootObject getLootObject(int id)
  {
    return lootObjects[id];
  }
};

// Helper function for fetching game objects
std::vector<YieldableItem> fetchYieldableItemList(GameItems &gameItems, const rapidjson::Value &object)
{
  std::vector<YieldableItem> yieldableItemList;

  for (auto &yieldableItem : object["yieldableItemList"].GetArray())
  {
    if (!yieldableItem.IsObject() || !validMember(yieldableItem, "itemId", ValueType::INT) || !validMember(yieldableItem, "yieldPercentage", ValueType::INT))
    {
      throw std::runtime_error("Object JSON file contains invalid yieldable item");
    }

    const int itemId = yieldableItem["itemId"].GetInt();

    if (!gameItems.containsId(itemId))
    {
      throw std::runtime_error("No item with id " + std::to_string(itemId) + " found when loading object");
    }

    const int yieldPercentage = yieldableItem["yieldPercentage"].GetInt();

    YieldableItem yItem;
    yItem.itemId = itemId;
    yItem.yieldPercentage = yieldPercentage;
    yieldableItemList.push_back(yItem);
  }

  return yieldableItemList;
}

// Reads objects defined in the objects.json file and returns them as a GameObjects struct
GameObjects fetchObjects(GameItems &gameItems, std::map<int, GameSkill> &gameSkills)
{

  rapidjson::Document document = readJson(OBJECT_JSON_PATH);

  if (!document.IsObject() || !document.HasMember("objects") || !document["objects"].IsArray())
  {
    throw std::runtime_error("Invalid JSON structure. Object JSON file does not contain objects array");
  }

  GameObjects gameObjects;

  for (rapidjson::Value::ConstValueIterator itr = document["objects"].Begin(); itr != document["objects"].End(); ++itr)
  {
    const rapidjson::Value &object = *itr;

    // Validate common object members
    if (!validMember(object, "type", ValueType::STRING) || !validMember(object, "id", ValueType::INT) || !validMember(object, "name", ValueType::STRING) || !validMember(object, "description", ValueType::STRING) || !validMember(object, "tileMap", ValueType::ARRAY))
    {
      throw std::runtime_error("Object JSON file contains invalid object");
    }

    const int objectId = object["id"].GetInt();
    const std::string objectType = object["type"].GetString();
    const std::string objectName = object["name"].GetString();
    const std::string objectDescription = object["description"].GetString();

    std::cout << objectType << " Object ID " << objectId << ", Name: " << objectName << std::endl;

    std::vector<std::vector<std::string>> tileMap;
    for (auto &tileRow : object["tileMap"].GetArray())
    {
      if (!tileRow.IsArray())
      {
        throw std::runtime_error("Tile map row in object JSON file is not an array");
      }

      std::vector<std::string> tileRowVector;
      for (auto &tile : tileRow.GetArray())
      {
        if (!tile.IsString())
        {
          throw std::runtime_error("Tile in object JSON file is not a string");
        }

        tileRowVector.push_back(tile.GetString());
      }
      tileMap.push_back(tileRowVector);
    }

    if (objectType == "General")
    {
      GeneralObject generalObject;
      generalObject.id = objectId;
      generalObject.name = objectName;
      generalObject.description = objectDescription;
      generalObject.tileMap = tileMap;

      gameObjects.generalObjects[objectId] = generalObject;
    }
    else if (objectType == "Resource")
    {
      if (!validMember(object, "yieldableItemList", ValueType::ARRAY) || !validMember(object, "xpPerYield", ValueType::INT) || !validMember(object, "depleteChance", ValueType::INT) || !validMember(object, "relatedSkillId", ValueType::INT) || !validMember(object, "xpRequirement", ValueType::INT))
      {
        throw std::runtime_error("Object JSON file contains invalid resource object");
      }
      const int xpPerYield = object["xpPerYield"].GetInt();
      const int depleteChance = object["depleteChance"].GetInt();
      const int relatedSkillId = object["relatedSkillId"].GetInt();
      const int xpRequirement = object["xpRequirement"].GetInt();

      if (!gameSkills.contains(relatedSkillId))
      {
        throw std::runtime_error("No skill with id " + std::to_string(relatedSkillId) + " found when loading object");
      }

      std::vector<YieldableItem> yieldableItemList = fetchYieldableItemList(gameItems, object);

      ResourceObject resourceObject;
      resourceObject.id = objectId;
      resourceObject.name = objectName;
      resourceObject.description = objectDescription;
      resourceObject.tileMap = tileMap;
      resourceObject.xpPerYield = xpPerYield;
      resourceObject.depleteChance = depleteChance;
      resourceObject.relatedSkillId = relatedSkillId;
      resourceObject.xpRequirement = xpRequirement;

      gameObjects.resourceObjects[objectId] = resourceObject;
    }
    else if (objectType == "Loot")
    {
      if (!validMember(object, "yieldableItemList", ValueType::ARRAY))
      {
        throw std::runtime_error("Object JSON file contains invalid loot object");
      }

      std::vector<YieldableItem> yieldableItemList = fetchYieldableItemList(gameItems, object);

      LootObject lootObject;
      lootObject.id = objectId;
      lootObject.name = objectName;
      lootObject.description = objectDescription;
      lootObject.tileMap = tileMap;
      lootObject.yieldableItemList = yieldableItemList;

      gameObjects.lootObjects[objectId] = lootObject;
    }
    else
    {
      throw std::runtime_error("Object JSON file contains invalid object type" + objectType);
    }
  }

  return gameObjects;
}
