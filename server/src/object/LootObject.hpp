#pragma once

#include <optional>
#include <vector>

#include "Object.hpp"

class LootObject : public Object
{
public:
    LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, reader::ObjectType type);

    LootObject(const LootObjectStruct& object, const ObjectLocation& location, reader::ObjectType objectType);

    /**
     * @brief Interact with the object.
     * @param entity that interacts with the object.
     */
    void interact(Entity& entity);

private:
    std::vector<YieldableItem> yieldableItemListM;
};
