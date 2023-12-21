#include "Equipment.hpp"

#include "../entity/Entity.hpp"

Equipment::Equipment(Entity* owner) : ownerM(owner), slotMapM{}
{
    slotMapM = {
        {Slot::HEAD, headSlotM},
        {Slot::NECK, neckSlotM},
        {Slot::TOP, topSlotM},
        {Slot::MAIN_HAND, mainHandSlotM},
        {Slot::OFF_HAND, offHandSlotM},
        {Slot::BOTTOM, bottomSlotM},
        {Slot::BOOTS, bootsSlotM},
        {Slot::GLOVES, glovesSlotM},
        {Slot::RING, ringSlotM},
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

std::vector<std::shared_ptr<EquippableItem>> Equipment::getAllEquipment()
{
    std::vector<std::shared_ptr<EquippableItem>> res;
    for (auto entry : slotMapM)
    {
        if (entry.second != nullptr)
        {
            res.push_back(entry.second);
        }
    }
    return res;
}

std::map<Slot, std::shared_ptr<EquippableItem>>& Equipment::getSlotMap()
{
    return slotMapM;
}

bool Equipment::equip(std::shared_ptr<EquippableItem> item)
{
    if (item == nullptr)
    {
        return false;
    }

    auto& currentItem = slotMapM[item->getEquippableSlot()];
    if (currentItem != nullptr)
    {
        ownerM->getInventory().addItem(currentItem);
    }
    currentItem = item;
    return true;
}

void Equipment::unequip(Slot slotToUnequip)
{
    if (slotMapM.contains(slotToUnequip))
    {
        auto& currentItem = slotMapM[slotToUnequip];
        if (ownerM->getInventory().addItem(currentItem))
        {
            currentItem = nullptr;
        }
    }
}