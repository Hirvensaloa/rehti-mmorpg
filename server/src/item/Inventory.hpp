#pragma once

#include "FoodItem.hpp"

#include <memory>
#include <vector>

class Inventory
{
public:
    Inventory(Entity *owner, int inventorySize = 28);

    const std::vector<std::shared_ptr<Item>> &getItems() const;

    bool addItem(std::shared_ptr<Item> item);

    bool removeItem(int itemId);

    void useItem(int itemId);

    friend std::ostream &operator<<(std::ostream &os, const Inventory &inv);

private:
    Entity *ownerM;

    int inventorySizeM;

    std::vector<std::shared_ptr<Item>> itemsM = {};
};