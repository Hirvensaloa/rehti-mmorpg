#include "ResourceObject.hpp"
#include "../utils/AssetManager.hpp"

ResourceObject::ResourceObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, int xpPerYield, int depleteChance, int relatedSkillId, int xpRequirement, ObjectType type) : Object(id, instanceId, name, coords, rotation, type)
{
  this->yieldableItemListM = yieldableItemList;
  this->xpPerYieldM = xpPerYield;
  this->depleteChanceM = depleteChance;
  this->relatedSkillIdM = relatedSkillId;
  this->xpRequirementM = xpRequirement;
}

ResourceObject::ResourceObject(const ResourceObjectStruct &object, const ObjectLocation &location, ObjectType type) : Object(object.id, location.instanceId, object.name, {location.x, location.y}, location.rotation, type)
{
  this->yieldableItemListM = object.yieldableItemList;
  this->xpPerYieldM = object.xpPerYield;
  this->depleteChanceM = object.depleteChance;
  this->relatedSkillIdM = object.relatedSkillId;
  this->xpRequirementM = object.xpRequirement;
}

void ResourceObject::interact(Entity &entity)
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

      entity.getSkillSet().addSkillXp(relatedSkillIdM, xpPerYieldM);
      std::cout << "Entity " << entity.getName() << " received " << itemPtr->getName() << " from " << getName() << " and " << xpPerYieldM << " xp" << std::endl;
      return;
    }
    cumulativeChance += item.yieldPercentage;
  }

  return;
}