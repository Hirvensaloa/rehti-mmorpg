#pragma once

#include "FoodItem.hpp"

#include <memory>
#include <vector>

/**
 * @brief Represents the items that an entity can carry
 */
class Inventory
{
public:
    Inventory(Entity* owner);

    /**
     * @brief Returns the items that the inventory contains
     * @return const std::vector<std::shared_ptr<Item>>&
     */
    const std::vector<std::shared_ptr<Item>> getItems() const;

    /**
     * @brief Adds item to the inventory.
     *
     * @param item shared pointer to Item
     * @return true if the item was added, false if not.
     */
    bool addItem(std::shared_ptr<Item> item);

    /**
     * @brief Removes item from the inventory.
     * @param itemId instance id of item to be removed
     * @return true if item was successfully removed, false if not
     */
    std::shared_ptr<Item> removeItem(int itemId);

    /**
     * @brief Removes all items from the inventory.
     */
    void removeAllItems();

    /**
     * @brief Uses an item in the inventory
     * @param itemId instance id of item to be used
     */
    void useItem(int itemId);

    /**
     * @brief Checks if inventory is full.
     *
     * @return true if inventory is full, false if not.
     */
    bool isFull() const;

    /**
     * @brief Prints the contents of the inventory to ostream
     * @param os ostream where to print
     * @param inv inventory to be printed
     * @return os
     */
    friend std::ostream& operator<<(std::ostream& os, const Inventory& inv);

private:
    Entity* ownerM;

    const int inventorySizeM = 28;

    std::vector<std::shared_ptr<Item>> itemsM = {};
};