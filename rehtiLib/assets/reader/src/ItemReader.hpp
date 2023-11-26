#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <string>

struct ItemStats
{
    ItemStats(int acc = 0, int dam = 0, int aspd = 0, int rng = 0, int arm = 0, int dod = 0) : accuracy(acc), damage(dam), attackSpeed(aspd), range(rng), armor(arm), dodge(dod){};

    int accuracy; // accuracy bonus

    int damage; // damage bonus

    int attackSpeed; // attack speed in milliseconds, non-zero only for weapons

    int range; // range in tiles, non-zero only for weapons

    int armor; // armor bonus

    int dodge; // dodge change bonus

    ItemStats operator+(ItemStats other)
    {
        return ItemStats(accuracy + other.accuracy, damage + other.damage, std::max(attackSpeed, other.attackSpeed), std::max(range, other.range), armor + other.armor, dodge + other.dodge);
    }

    friend std::ostream& operator<<(std::ostream& os, const ItemStats& stats);
};

inline std::ostream& operator<<(std::ostream& os, const ItemStats& stats)
{
    os << "ItemStats: accuracy = " << stats.accuracy << ", damage = " << stats.damage << ", attackSpeed = " << stats.attackSpeed << ", range = " << stats.range << ", armor = " << stats.armor << ", dodge = " << stats.dodge << std::endl;
    return os;
};

enum class Slot
{
    HEAD,
    NECK,
    TOP,
    MAIN_HAND,
    OFF_HAND,
    BOTTOM,
    GLOVES,
    BOOTS,
    RING,
};

struct GeneralItemStruct
{
    int id;
    std::string name;
    std::string description;
    bool isStackable;
};

struct EquippableItemStruct
{
    int id;
    std::string name;
    std::string description;
    Slot slot;
    ItemStats stats;
    bool isStackable;
};

struct FoodItemStruct
{
    int id;
    std::string name;
    std::string description;
    int healAmount;
    bool isStackable;
};

enum class ItemType
{
    GENERAL,
    EQUIPPABLE,
    FOOD
};

struct GameItems
{
    std::map<int, GeneralItemStruct> generalItems;
    std::map<int, EquippableItemStruct> equippableItems;
    std::map<int, FoodItemStruct> foodItems;

    const bool containsId(const int id) const
    {
        return generalItems.contains(id) || equippableItems.contains(id) || foodItems.contains(id);
    }

    std::optional<ItemType> getItemType(int id)
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
            return std::nullopt;
        }
    }

    const GeneralItemStruct& getGeneralItem(int id)
    {
        return generalItems.at(id);
    }

    const EquippableItemStruct& getEquippableItem(int id)
    {
        return equippableItems.at(id);
    }

    const FoodItemStruct& getFoodItem(int id)
    {
        return foodItems.at(id);
    }
};

// Reads items defined in the items.json file and returns them as a GameItems struct. GameItems can be referred to create item instances to the game (based on the id).
GameItems fetchItems();