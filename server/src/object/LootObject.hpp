#pragma once

#include <optional>
#include <vector>

#include "Object.hpp"

/**
 * @brief Represents an object that can be looted
 */
class LootObject : public Object
{
public:
    LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, reader::ObjectType type);

    LootObject(const LootObjectStruct& object, const ObjectLocation& location, reader::ObjectType objectType);

    /**
     * @brief Check if the entity can interact with the object.
     * @param entity that interacts with the object.
     * @return True if the entity can interact with the object.
     */
    bool canInteract(Entity& entity);

    /**
     * @brief Interact with the object.
     * @param entity that interacts with the object.
     */
    void interact(Entity& entity);

private:
    std::vector<YieldableItem> yieldableItemListM;
};
