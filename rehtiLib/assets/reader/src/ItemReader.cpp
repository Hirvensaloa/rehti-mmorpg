#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../../Config.hpp"
#include "ItemReader.hpp"
#include "RehtiUtils.hpp"

GameItems fetchItems()
{
    rapidjson::Document document = readJson(Config.ITEM_JSON_PATH);

    if (!document.IsObject() || !validMember(document, "items", ValueType::ARRAY))
    {
        throw std::runtime_error("Invalid JSON structure.");
    }

    const rapidjson::Value& itemsArray = document["items"];

    GameItems gameItems;

    for (rapidjson::SizeType i = 0; i < itemsArray.Size(); i++)
    {
        const rapidjson::Value& item = itemsArray[i];
        if (!item.IsObject() || !validMember(item, "type", ValueType::STRING) || !validMember(item, "id", ValueType::INT))
        {
            std::cerr << "JSON Validation failed for item" << std::endl;
            throw std::runtime_error("JSON Validation failed for item index: " + std::to_string(i));
        }

        const std::string itemType = item["type"].GetString();
        int itemId = item["id"].GetInt();

        // Validate common item properties (= validate the general item)
        if (!validMember(item, "name", ValueType::STRING) ||
            !validMember(item, "description", ValueType::STRING) ||
            !validMember(item, "isStackable", ValueType::BOOL) ||
            !validMember(item, "textureFilename", ValueType::STRING) ||
            !validMember(item, "objFilename", ValueType::STRING) ||
            !validMember(item, "iconFilename", ValueType::STRING))
        {
            throw std::runtime_error("JSON Validation failed for item id: " + std::to_string(itemId));
        }

        // Validate file existence
        const std::string textureFilename = item["textureFilename"].GetString();
        const std::string objFilename = item["objFilename"].GetString();
        const std::string iconFilename = item["iconFilename"].GetString();

        if (!std::filesystem::exists(Config.TEXTURE_PATH + textureFilename))
        {
            throw std::runtime_error("Texture file not found: " + textureFilename);
        }

        if (!std::filesystem::exists(Config.OBJECT_OBJ_PATH + objFilename))
        {
            throw std::runtime_error("Obj file not found: " + objFilename);
        }

        if (!std::filesystem::exists(Config.ITEM_ICON_PATH + iconFilename))
        {
            throw std::runtime_error("Icon file not found: " + iconFilename);
        }

        if (gameItems.containsId(itemId))
        {
            throw std::runtime_error("Duplicate item id: " + std::to_string(itemId));
        }

        if (itemType == "General")
        {
            GeneralItemStruct generalItem;
            generalItem.id = itemId;
            generalItem.name = item["name"].GetString();
            generalItem.description = item["description"].GetString();
            generalItem.isStackable = item["isStackable"].GetBool();
            generalItem.textureFilename = textureFilename;
            generalItem.objFilename = objFilename;
            generalItem.iconFilename = iconFilename;

            gameItems.generalItems[itemId] = generalItem;
        }
        else if (itemType == "Equippable")
        {
            if (!validMember(item, "slot", ValueType::STRING) || !validMember(item, "stats", ValueType::OBJECT))
            {
                throw std::runtime_error("JSON Validation failed for item id: " + std::to_string(itemId));
            }

            EquippableItemStruct equippableItem;
            equippableItem.id = itemId;
            equippableItem.name = item["name"].GetString();
            equippableItem.description = item["description"].GetString();
            equippableItem.textureFilename = textureFilename;
            equippableItem.objFilename = objFilename;
            equippableItem.iconFilename = iconFilename;
            std::string slot = item["slot"].GetString();

            if (slot == "MainHand")
            {
                equippableItem.slot = Slot::MAIN_HAND;
            }
            else if (slot == "OffHand")
            {
                equippableItem.slot = Slot::OFF_HAND;
            }
            else if (slot == "Head")
            {
                equippableItem.slot = Slot::HEAD;
            }
            else if (slot == "Top")
            {
                equippableItem.slot = Slot::TOP;
            }
            else if (slot == "Bottom")
            {
                equippableItem.slot = Slot::BOTTOM;
            }
            else if (slot == "Boots")
            {
                equippableItem.slot = Slot::BOOTS;
            }
            else if (slot == "Gloves")
            {
                equippableItem.slot = Slot::GLOVES;
            }
            else if (slot == "Neck")
            {
                equippableItem.slot = Slot::NECK;
            }
            else if (slot == "Ring")
            {
                equippableItem.slot = Slot::RING;
            }
            else
            {
                throw std::runtime_error("Unknown slot: " + slot);
            }

            const rapidjson::Value& stats = item["stats"];

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

            FoodItemStruct foodItem;
            foodItem.id = itemId;
            foodItem.name = item["name"].GetString();
            foodItem.description = item["description"].GetString();
            foodItem.healAmount = item["healAmount"].GetInt();
            foodItem.isStackable = item["isStackable"].GetBool();
            foodItem.textureFilename = textureFilename;
            foodItem.objFilename = objFilename;
            foodItem.iconFilename = iconFilename;

            gameItems.foodItems[itemId] = foodItem;
        }
        else
        {
            throw std::runtime_error("Unknown item type");
        }
    }

    for (const auto& pair : gameItems.generalItems)
    {
        const GeneralItemStruct& item = pair.second;
        std::cout << "General Item ID: " << item.id << ", Name: " << item.name << std::endl;
    }

    for (const auto& pair : gameItems.equippableItems)
    {
        const EquippableItemStruct& item = pair.second;
        std::cout << "Equippable Item ID: " << item.id << ", Name: " << item.name << std::endl;
    }

    for (const auto& pair : gameItems.foodItems)
    {
        const FoodItemStruct& item = pair.second;
        std::cout << "Food Item ID: " << item.id << ", Name: " << item.name << std::endl;
    }

    std::cout << "Items fetched successfully!" << std::endl;

    return gameItems;
}