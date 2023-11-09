#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "ObjectReader.hpp"
#include "ItemReader.hpp"
#include "RehtiUtils.hpp"
#include "Utils.hpp"
#include "../../Config.hpp"

std::string generateObjectInstanceId(ObjectLocation objectLocation)
{
  std::string idStr = std::to_string(objectLocation.id);
  std::string xStr = std::to_string(objectLocation.x);
  std::string yStr = std::to_string(objectLocation.y);
  std::string zStr = std::to_string(objectLocation.z);

  // Todo: Add - between id and location to ensure that this is unique
  std::string id = idStr + xStr + yStr + zStr;

  return id;
};

const std::vector<ObjectLocation> readObjectLocations()
{
  rapidjson::Document objectsJson = readJson(Config.GENERATED_OBJECT_JSON_PATH);

  if (!objectsJson.IsObject() || !validMember(objectsJson, "objects", ValueType::ARRAY))
  {
    throw std::runtime_error("Invalid JSON structure.");
  }

  const rapidjson::Value &objArray = objectsJson["objects"];

  std::vector<ObjectLocation> objects;

  for (const auto &object : objArray.GetArray())
  {
    if (!validMember(object, "id", ValueType::INT) || !validMember(object, "x", ValueType::INT) || !validMember(object, "y", ValueType::INT) || !validMember(object, "z", ValueType::INT) || !validMember(object, "rotation", ValueType::INT))
    {
      throw std::runtime_error("Object JSON file contains invalid object");
    }

    ObjectLocation objectLocation;
    objectLocation.id = object["id"].GetUint();
    objectLocation.x = object["x"].GetUint();
    objectLocation.y = object["y"].GetUint();
    objectLocation.z = object["z"].GetUint();
    objectLocation.instanceId = generateObjectInstanceId(objectLocation);
    objectLocation.rotation = object["rotation"].GetInt();

    objects.push_back(objectLocation);
  }

  return objects;
}

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
};

GameObjects fetchObjects(GameItems &gameItems, std::map<int, GameSkill> &gameSkills)
{

  rapidjson::Document document = readJson(Config.OBJECT_JSON_PATH);

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
      GeneralObjectStruct generalObject;
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

      ResourceObjectStruct resourceObject;
      resourceObject.id = objectId;
      resourceObject.name = objectName;
      resourceObject.description = objectDescription;
      resourceObject.tileMap = tileMap;
      resourceObject.xpPerYield = xpPerYield;
      resourceObject.depleteChance = depleteChance;
      resourceObject.relatedSkillId = relatedSkillId;
      resourceObject.xpRequirement = xpRequirement;
      resourceObject.yieldableItemList = yieldableItemList;

      gameObjects.resourceObjects[objectId] = resourceObject;
    }
    else if (objectType == "Loot")
    {
      if (!validMember(object, "yieldableItemList", ValueType::ARRAY))
      {
        throw std::runtime_error("Object JSON file contains invalid loot object");
      }

      std::vector<YieldableItem> yieldableItemList = fetchYieldableItemList(gameItems, object);

      LootObjectStruct lootObject;
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
};
