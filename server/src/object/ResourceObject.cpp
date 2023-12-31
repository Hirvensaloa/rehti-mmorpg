#include "ResourceObject.hpp"
#include "../utils/AssetManager.hpp"

ResourceObject::ResourceObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, int xpPerYield, int depleteChance, int relatedSkillId, int xpRequirement, reader::ObjectType type, std::vector<ItemTransform> itemTransformList) : Object(id, instanceId, name, coords, rotation, type)
{
    this->yieldableItemListM = yieldableItemList;
    this->xpPerYieldM = xpPerYield;
    this->depleteChanceM = depleteChance;
    this->relatedSkillIdM = relatedSkillId;
    this->xpRequirementM = xpRequirement;
    this->itemTransformListM = itemTransformList;
}

ResourceObject::ResourceObject(const ResourceObjectStruct& object, const ObjectLocation& location, reader::ObjectType type) : Object(object.id, location.instanceId, object.name, {location.x, location.y}, location.rotation, type)
{
    this->yieldableItemListM = object.yieldableItems.yieldableItemList;
    this->xpPerYieldM = object.yieldableItems.xpPerYield;
    this->depleteChanceM = object.depleteChance;
    this->relatedSkillIdM = object.relatedSkillId;
    this->xpRequirementM = object.xpRequirement;
    this->itemTransformListM = object.itemTransformList;
}

bool ResourceObject::canInteract(Entity& entity)
{
    // Check that player matches the xp requirement
    const auto skill = entity.getSkillSet().getSkills().at(relatedSkillIdM);
    if (skill.xp < xpRequirementM)
    {
        std::cout << "Entity " << entity.getName() << " does not have enough xp to interact with " << getName() << std::endl;
        return false;
    }

    // Yieldable items have priority over item transforms. If yieldable items is not empty, we need to check if the entity has enough inventory space
    if (!this->yieldableItemListM.empty())
    {
        if (entity.getInventory().isFull())
        {
            std::cout << "Entity " << entity.getName() << " does not have enough inventory space to interact with " << getName() << std::endl;
            return false;
        }
        else
        {
            return true;
        }
    }
    else if (!this->itemTransformListM.empty())
    {
        // We need to check if the entity has any of the items that can be transformed
        Inventory& inventory = entity.getInventory();
        const auto& items = inventory.getItems();
        for (ItemTransform itemTransform : itemTransformListM)
        {
            for (const auto& item : items)
            {
                if (item->getId() == itemTransform.itemId)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void ResourceObject::interact(Entity& entity)
{
    if (!canInteract(entity))
    {
        std::cout << "Entity " << entity.getName() << " cannot interact with " << getName() << std::endl;
        return;
    }

    // Yieldable items have priority over item transforms
    if (!this->yieldableItemListM.empty())
    {
        int rand = std::rand() % 100;
        int cumulativeChance = 0;

        for (YieldableItem item : yieldableItemListM)
        {
            if (cumulativeChance < rand && rand < cumulativeChance + item.yieldPercentage)
            {
                const GameItems& items = AssetManager::getItems();
                if (!items.containsId(item.itemId))
                {
                    return;
                }

                Inventory& inventory = entity.getInventory();
                std::shared_ptr<Item> itemPtr = AssetManager::createItemInstance(item.itemId);
                inventory.addItem(itemPtr);

                entity.getSkillSet().addSkillXp(relatedSkillIdM, xpPerYieldM);
                std::cout << "Entity " << entity.getName() << " received " << itemPtr->getName() << " from " << getName() << " and " << xpPerYieldM << " xp" << std::endl;
                return;
            }
            cumulativeChance += item.yieldPercentage;
        }
    }
    else if (!this->itemTransformListM.empty())
    {
        // First fetch the player's inventory and check if it matches any itemId in the itemTransformList
        Inventory& inventory = entity.getInventory();
        const auto& items = inventory.getItems();
        for (ItemTransform itemTransform : itemTransformListM)
        {
            for (const auto& item : items)
            {
                if (item->getId() == itemTransform.itemId)
                {
                    int itemAmountToBeAdded = itemTransform.resultItemQuantity;

                    if (itemAmountToBeAdded < 1)
                    {
                        continue;
                    }

                    inventory.removeItem(item->getInstanceId());

                    int itemsAdded = 0;
                    std::string itemName = "";
                    while (itemAmountToBeAdded > 0)
                    {
                        std::shared_ptr<Item> resultItem = AssetManager::createItemInstance(itemTransform.resultItemId);
                        itemName = resultItem->getName();

                        const bool success = inventory.addItem(resultItem);

                        // Let's not do anything if we can't add the item to the inventory. We cannot be sure why it happens.
                        if (success)
                        {
                            itemsAdded++;
                        }

                        itemAmountToBeAdded--;
                    }

                    // Xp is given only once per transform
                    entity.getSkillSet().addSkillXp(relatedSkillIdM, itemTransform.xpPerTransform);
                    std::cout << "Entity " << entity.getName() << " transformed " << item->getName() << " into " << itemsAdded << " " << itemName << " and " << itemTransform.xpPerTransform << " xp" << std::endl;
                    return;
                }
            }
        }
    }

    return;
}