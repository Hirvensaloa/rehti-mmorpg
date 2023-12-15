#pragma once

#include "Item.hpp"
#include "RehtiReader.hpp"

#include <memory>

/**
 * @brief Represents an item that can be equipped
 */
class EquippableItem : public Item, public std::enable_shared_from_this<EquippableItem>
{
public:
    EquippableItem(int id, std::string name, Slot equippableToSlot, ItemStats stats = ItemStats(), bool isStackable = false);

    EquippableItem(const EquippableItemStruct& equippableItem);

    /**
     * @brief Returns the slot that the item can be equipped to
     * @return Slot
     */
    Slot getEquippableSlot();

    /**
     * @brief Returns the stats of the item
     * @return ItemStats&
     */
    ItemStats& getStats();

    /**
     * @brief Equips the item
     * @param user
     * @return bool
     */
    bool use(Entity* user) override;

protected:
    Slot equippableToSlotM;

    ItemStats statsM;
};