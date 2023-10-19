#pragma once

#include "EquippableItem.hpp"

#include <map>
#include <memory>

class Entity;

class Equipment
{

public:
    Equipment(Entity *owner);

    bool equip(std::shared_ptr<EquippableItem> item);

    void unequip(Slot slotToUnequip);

    ItemStats getEquipmentStats();

private:
    Entity *ownerM;

    std::map<Slot, std::shared_ptr<EquippableItem>>
        slotMapM;

    std::shared_ptr<EquippableItem> mainHandSlotM = nullptr;

    std::shared_ptr<EquippableItem> offHandSlotM = nullptr;

    std::shared_ptr<EquippableItem> headSlotM = nullptr;

    std::shared_ptr<EquippableItem> topSlotM = nullptr;

    std::shared_ptr<EquippableItem> bottomSlotM = nullptr;

    std::shared_ptr<EquippableItem> bootsSlotM = nullptr;

    std::shared_ptr<EquippableItem> glovesSlotM = nullptr;

    std::shared_ptr<EquippableItem> neckSlotM = nullptr;

    std::shared_ptr<EquippableItem> ringSlotM = nullptr;
};