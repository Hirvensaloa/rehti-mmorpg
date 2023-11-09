#include "LootObject.hpp"
#include "../utils/AssetManager.hpp"

LootObject::LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, reader::ObjectType type)
    : Object(id, instanceId, name, coords, rotation, type), yieldableItemListM(yieldableItemList)
{
}

LootObject::LootObject(const LootObjectStruct &object, const ObjectLocation &location, reader::ObjectType type)
    : Object(object.id, location.instanceId, object.name, {location.x, location.y}, location.rotation, type), yieldableItemListM(object.yieldableItemList)
{
}

void LootObject::interact(Entity &entity)
{
  int rand = std::rand() % 100;
  int cumulativeChance = 0;

  for (YieldableItem item : yieldableItemListM)
  {
    if (cumulativeChance < rand && rand < cumulativeChance + item.yieldPercentage)
    {
      const GameItems &items = AssetManager::getItems();
      if (!items.containsId(item.itemId))
      {
        return;
      }

      Inventory &inventory = entity.getInventory();
      std::shared_ptr<Item> itemPtr = AssetManager::createItemInstance(item.itemId);
      inventory.addItem(itemPtr);

      std::cout << "Player " << entity.getName() << " looted " << itemPtr->getName() << " from " << getName() << std::endl;
      return;
    }
    cumulativeChance += item.yieldPercentage;
  }

  return;
}
