#pragma once

#include "FoodItem.hpp"

#include <memory>
#include <vector>

class Inventory
{
public:
    Inventory(Entity* owner, int inventorySize = 28);

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

    friend std::ostream& operator<<(std::ostream& os, const Inventory& inv);

private:
    Entity* ownerM;

    int inventorySizeM;

    std::vector<std::shared_ptr<Item>> itemsM = {};
};