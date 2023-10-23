#include "EquippableItem.hpp"

#include "../entity/Entity.hpp"

EquippableItem::EquippableItem(int id, std::string name, Slot equippableToSlot, ItemStats stats, bool isStackable) : Item(id, name, isStackable), equippableToSlotM(equippableToSlot), statsM(stats){};

Slot EquippableItem::getEquippableSlot()
{
    return equippableToSlotM;
}

ItemStats &EquippableItem::getStats()
{
    return statsM;
}

bool EquippableItem::use(Entity *user)
{
    return user->getEquipment().equip(this->shared_from_this());
}