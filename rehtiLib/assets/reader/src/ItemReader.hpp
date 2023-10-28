#pragma once

#include <string>
#include <iostream>
#include <map>

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
const std::string ITEM_JSON_PATH = "./assets/items.json";

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
GameItems fetchItems();