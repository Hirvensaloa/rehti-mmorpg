#include "AssetManager.hpp"
#include "../item/EquippableItem.hpp"
#include "../item/FoodItem.hpp"
#include "../object/LootObject.hpp"
#include "../object/ResourceObject.hpp"
#include "../object/Object.hpp"

void AssetManager::loadAssets()
{
  std::cout << ">>>>>> Loading assets..."
            << std::endl;

  itemsM = fetchItems();
  skillsM = fetchSkills();
  objectsM = fetchObjects(itemsM, skillsM);
  objectLocationsM = readObjectLocations();

  std::cout << ">>>>>> Assets loaded successfully!\n"
            << std::endl;
};

const std::vector<ObjectLocation> &AssetManager::getObjectLocations()
{
  return objectLocationsM;
};

const std::map<int, GameSkill> &AssetManager::getSkills()
{
  return skillsM;
};

const GameObjects &AssetManager::getObjects()
{
  return objectsM;
};

const GameItems &AssetManager::getItems()
{
  return itemsM;
};

std::shared_ptr<Item> AssetManager::createItemInstance(int id)
{
  if (itemsM.containsId(id))
  {
    std::optional<ItemType> type = itemsM.getItemType(id);
    if (type == std::nullopt)
    {
      return nullptr;
    }

    switch (type.value())
    {
    case ItemType::GENERAL:
    {
      const GeneralItemStruct &generalItemStruct = itemsM.getGeneralItem(id);
      return std::make_shared<Item>(Item(generalItemStruct));
    }
    case ItemType::EQUIPPABLE:
    {
      const EquippableItemStruct &equippableItemStruct = itemsM.getEquippableItem(id);
      return std::make_shared<EquippableItem>(EquippableItem(equippableItemStruct));
    }
    case ItemType::FOOD:
    {
      const FoodItemStruct &foodItemStruct = itemsM.getFoodItem(id);
      return std::make_shared<FoodItem>(FoodItem(foodItemStruct));
    }
    default:
      return nullptr;
    }
  }
  else
  {
    return nullptr;
  }
};
