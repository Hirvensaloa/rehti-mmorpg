#include "Equipment.hpp"

#include "../entity/Entity.hpp"

Equipment::Equipment(Entity *owner) : ownerM(owner), slotMapM{}
{
    slotMapM = {
        {Slot::MainHand, mainHandSlotM},
        {Slot::OffHand, offHandSlotM},
        {Slot::Head, headSlotM},
        {Slot::Top, topSlotM},
        {Slot::Bottom, bottomSlotM},
        {Slot::Boots, bootsSlotM},
        {Slot::Gloves, glovesSlotM},
        {Slot::Neck, neckSlotM},
        {Slot::Ring, ringSlotM},
    };
};

ItemStats Equipment::getEquipmentStats()
{
    ItemStats stats = ItemStats();

    for (auto slot : slotMapM)
    {
        if (slot.second != nullptr)
        {
            stats = stats + slot.second->getStats();
        }
    }
    return stats;
}

bool Equipment::equip(std::shared_ptr<EquippableItem> item)
{
    if (item == nullptr)
    {
        return false;
    }

    auto &currentItem = slotMapM[item->getEquippableSlot()];
    if (currentItem != nullptr)
    {
        ownerM->getInventory().addItem(currentItem);
    }
    currentItem = item;
    return true;
}

void Equipment::unequip(Slot slotToUnequip)
{
    auto &currentItem = slotMapM[slotToUnequip];
    if (ownerM->getInventory().addItem(currentItem))
    {
        currentItem = nullptr;
    }
}