#pragma once

#include "EquippableItem.hpp"

#include <map>
#include <memory>

class Entity;

/**
 * @brief Class for representing the items an entity has equipped
 */
class Equipment
{

public:
    Equipment(Entity* owner);

    /**
     * @brief Equip an EquippableItem to the proper slot
     * @param item
     * @return Whether the operation was successful
     */
    bool equip(std::shared_ptr<EquippableItem> item);

    /**
     * @brief Unequip item from slot
     * @param slotToUnequip
     */
    void unequip(Slot slotToUnequip);

    /**
     * @brief Get combined stats of equipped items
     * @return ItemStats struct with the combined stats
     */
    ItemStats getEquipmentStats();

    /**
     * @brief Get all equipped items in vector
     * @return Vector of equipped items
     */
    std::vector<std::shared_ptr<EquippableItem>> getAllEquipment();

    /**
     * @brief Get the slot map which maps item slots to the items that are equipped in the slots
     * @return std::map<Slot, std::shared_ptr<EquippableItem>>&
     */
    std::map<Slot, std::shared_ptr<EquippableItem>>& getSlotMap();

private:
    Entity* ownerM;

    std::map<Slot, std::shared_ptr<EquippableItem>> slotMapM;

    std::shared_ptr<EquippableItem> headSlotM = nullptr;

    std::shared_ptr<EquippableItem> neckSlotM = nullptr;

    std::shared_ptr<EquippableItem> topSlotM = nullptr;

    std::shared_ptr<EquippableItem> mainHandSlotM = nullptr;

    std::shared_ptr<EquippableItem> offHandSlotM = nullptr;

    std::shared_ptr<EquippableItem> bottomSlotM = nullptr;

    std::shared_ptr<EquippableItem> bootsSlotM = nullptr;

    std::shared_ptr<EquippableItem> glovesSlotM = nullptr;

    std::shared_ptr<EquippableItem> ringSlotM = nullptr;
};