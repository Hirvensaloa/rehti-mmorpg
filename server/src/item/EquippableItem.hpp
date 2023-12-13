#pragma once

#include "Item.hpp"
#include "RehtiReader.hpp"

#include <memory>

class EquippableItem : public Item, public std::enable_shared_from_this<EquippableItem>
{
public:
    EquippableItem(int id, std::string name, Slot equippableToSlot, ItemStats stats = ItemStats(), bool isStackable = false);

    EquippableItem(const EquippableItemStruct &equippableItem);

    Slot getEquippableSlot();

    ItemStats &getStats();

    bool use(Entity *user) override;

protected:
    Slot equippableToSlotM;

    ItemStats statsM;
};