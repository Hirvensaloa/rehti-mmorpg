#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "RapidjsonHelpers.hpp"

struct ItemStats
{
  int accuracy;
  int damage;
  int attackSpeed;
  int range;
  int armor;
  int dodge;
};

struct GeneralItem
{
  int id;
  std::string name;
  std::string description;
  bool isStackable;
};

struct EquippableItem
{
  int id;
  std::string name;
  std::string description;
  std::string slot;
  ItemStats stats;
  bool isStackable;
};

struct FoodItem
{
  int id;
  std::string name;
  std::string description;
  int healAmount;
  bool isStackable;
};

// Path(s)
const std::string ITEM_JSON_PATH = "../../../assets/items.json";

enum class ItemType
{
  GENERAL,
  EQUIPPABLE,
  FOOD,
  NOT_FOUND
};

struct GameItems
{
  std::map<int, GeneralItem> generalItems;
  std::map<int, EquippableItem> equippableItems;
  std::map<int, FoodItem> foodItems;

  bool containsId(int id)
  {
    return generalItems.contains(id) || equippableItems.contains(id) || foodItems.contains(id);
  }

  ItemType findItemType(int id)
  {
    if (generalItems.contains(id))
    {
      return ItemType::GENERAL;
    }
    else if (equippableItems.contains(id))
    {
      return ItemType::EQUIPPABLE;
    }
    else if (foodItems.contains(id))
    {
      return ItemType::FOOD;
    }
    else
    {
      return ItemType::NOT_FOUND;
    }
  }

  const GeneralItem &getGeneralItem(int id)
  {
    return generalItems[id];
  }

  const EquippableItem &getEquippableItem(int id)
  {
    return equippableItems[id];
  }

  const FoodItem &getFoodItem(int id)
  {
    return foodItems[id];
  }
};

// Reads items defined in the items.json file and returns them as a GameItems struct. GameItems can be referred to create item instances to the game (based on the id).
GameItems fetchItems()
{
  rapidjson::Document document = readJson(ITEM_JSON_PATH);

  if (!document.IsObject() || !document.HasMember("items") || !document["items"].IsArray())
  {
    throw std::runtime_error("Invalid JSON structure.");
  }

  const rapidjson::Value &itemsArray = document["items"];

  GameItems gameItems;

  for (rapidjson::SizeType i = 0; i < itemsArray.Size(); i++)
  {
    const rapidjson::Value &item = itemsArray[i];
    if (!item.IsObject() || !validMember(item, "type", ValueType::STRING) || !validMember(item, "id", ValueType::INT))
    {
      std::cerr << "JSON Validation failed for item" << std::endl;
      throw std::runtime_error("JSON Validation failed for item index: " + std::to_string(i));
    }

    const std::string itemType = item["type"].GetString();
    int itemId = item["id"].GetInt();

    // Validate common item properties (= validate the general item)
    if (!validMember(item, "name", ValueType::STRING) || !validMember(item, "description", ValueType::STRING) || !validMember(item, "isStackable", ValueType::BOOL))
    {
      throw std::runtime_error("JSON Validation failed for item id: " + std::to_string(itemId));
    }

    if (gameItems.containsId(itemId))
    {
      throw std::runtime_error("Duplicate item id: " + std::to_string(itemId));
    }

    if (itemType == "General")
    {
      GeneralItem generalItem;
      generalItem.id = itemId;
      generalItem.name = item["name"].GetString();
      generalItem.description = item["description"].GetString();
      generalItem.isStackable = item["isStackable"].GetBool();

      gameItems.generalItems[itemId] = generalItem;
    }
    else if (itemType == "Equippable")
    {
      if (!validMember(item, "slot", ValueType::STRING) || !validMember(item, "stats", ValueType::OBJECT))
      {
        throw std::runtime_error("JSON Validation failed for item id: " + std::to_string(itemId));
      }

      EquippableItem equippableItem;
      equippableItem.id = itemId;
      equippableItem.name = item["name"].GetString();
      equippableItem.description = item["description"].GetString();
      equippableItem.slot = item["slot"].GetString();

      const rapidjson::Value &stats = item["stats"];

      // These fields are not required, if they are not found just set the stat to 0
      equippableItem.stats.accuracy = validMember(stats, "accuracy", ValueType::INT, false) ? stats["accuracy"].GetInt() : 0;
      equippableItem.stats.damage = validMember(stats, "damage", ValueType::INT, false) ? stats["damage"].GetInt() : 0;
      equippableItem.stats.attackSpeed = validMember(stats, "attackSpeed", ValueType::INT, false) ? stats["attackSpeed"].GetInt() : 0;
      equippableItem.stats.range = validMember(stats, "range", ValueType::INT, false) ? stats["range"].GetInt() : 0;
      equippableItem.stats.armor = validMember(stats, "armor", ValueType::INT, false) ? stats["armor"].GetInt() : 0;
      equippableItem.stats.dodge = validMember(stats, "dodge", ValueType::INT, false) ? stats["dodge"].GetInt() : 0;

      equippableItem.isStackable = item["isStackable"].GetBool();
      gameItems.equippableItems[itemId] = equippableItem;
    }
    else if (itemType == "Food")
    {
      if (!validMember(item, "healAmount", ValueType::INT))
      {
        throw std::runtime_error("JSON Validation failed for item id: " + std::to_string(itemId));
      }

      FoodItem foodItem;
      foodItem.id = itemId;
      foodItem.name = item["name"].GetString();
      foodItem.description = item["description"].GetString();
      foodItem.healAmount = item["healAmount"].GetInt();
      foodItem.isStackable = item["isStackable"].GetBool();

      gameItems.foodItems[itemId] = foodItem;
    }
    else
    {
      throw std::runtime_error("Unknown item type");
    }
  }

  for (const auto &pair : gameItems.generalItems)
  {
    const GeneralItem &item = pair.second;
    std::cout << "General Item ID: " << item.id << ", Name: " << item.name << std::endl;
  }

  for (const auto &pair : gameItems.equippableItems)
  {
    const EquippableItem &item = pair.second;
    std::cout << "Equippable Item ID: " << item.id << ", Name: " << item.name << std::endl;
  }

  for (const auto &pair : gameItems.foodItems)
  {
    const FoodItem &item = pair.second;
    std::cout << "Food Item ID: " << item.id << ", Name: " << item.name << std::endl;
  }

  std::cout << "Items fetched successfully!" << std::endl;

  return gameItems;
}