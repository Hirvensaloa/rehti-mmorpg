#pragma once

#include "Item.hpp"

/**
 * @brief Represents an item that can be eaten
 */
class FoodItem : public Item
{

public:
    FoodItem(int id, std::string name, int healAmount, bool isStackable = false);

    FoodItem(const FoodItemStruct& foodItem);

    /**
     * @brief Returns the amount of hp eating this item restores
     * @return int
     */
    int getHealAmount();

    /**
     * @brief Eats the item
     * @param user
     * @return bool
     */
    bool use(Entity* user) override;

private:
    int healAmountM;
};