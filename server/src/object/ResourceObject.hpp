#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../entity/Entity.hpp"
#include "Object.hpp"

class ResourceObject : public Object
{
public:
    ResourceObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, int xpPerYield, int depleteChance, int relatedSkillId, int xpRequirement, reader::ObjectType type, std::vector<ItemTransform> itemTransformList);

    ResourceObject(const ResourceObjectStruct& object, const ObjectLocation& location, reader::ObjectType type);

    /**
     * @brief Interact with the object.
     * @param entity that interacts with the object.
     */
    void interact(Entity& entity);

private:
    std::vector<YieldableItem> yieldableItemListM;
    int xpPerYieldM;
    int depleteChanceM; // TODO: Deplete functionality
    std::vector<ItemTransform> itemTransformListM;
    int relatedSkillIdM;
    int xpRequirementM;
};