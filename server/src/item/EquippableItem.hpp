#pragma once

#include "Item.hpp"
#include "ItemStats.hpp"

#include <memory>

enum class Slot
{
    MainHand,
    OffHand,
    Head,
    Top,
    Bottom,
    Boots,
    Gloves,
    Neck,
    Ring
};

class EquippableItem : public Item, public std::enable_shared_from_this<EquippableItem>
{
public:
    EquippableItem(int id, std::string name, Slot equippableToSlot, ItemStats stats = ItemStats(), bool isStackable = false);

    Slot getEquippableSlot();

    ItemStats &getStats();

    bool use(Entity *user) override;

protected:
    Slot equippableToSlotM;

    ItemStats statsM;
};