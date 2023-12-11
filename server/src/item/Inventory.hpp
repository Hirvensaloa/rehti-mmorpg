#pragma once

#include "FoodItem.hpp"

#include <memory>
#include <vector>

const int INVENTORY_SIZE = 28;

class Inventory
{
public:
    Inventory(Entity* owner);

    const std::vector<std::shared_ptr<Item>>& getItems() const;

    /**
     * @brief Adds item(s) to inventory.
     *
     * @param item shared pointer to Item
     * @return true if any one of the items was added, false if none were added.
     */
    bool addItem(std::shared_ptr<Item> item);

    std::shared_ptr<Item> removeItem(int itemId);

    void removeAllItems();

    void useItem(int itemId);

    /**
     * @brief Checks if inventory is full.
     *
     * @return true if inventory is full, false if not.
     */
    bool isFull() const;

    friend std::ostream& operator<<(std::ostream& os, const Inventory& inv);

private:
    Entity* ownerM;

    const int inventorySizeM = INVENTORY_SIZE;

    std::vector<std::shared_ptr<Item>> itemsM = {};
};