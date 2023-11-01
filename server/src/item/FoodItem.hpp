#pragma once

#include "Item.hpp"

class FoodItem : public Item
{

public:
    FoodItem(int id, std::string name, int healAmount, bool isStackable = false);

    FoodItem(const FoodItemStruct &foodItem);

    int getHealAmount();

    bool use(Entity *user) override;

private:
    int healAmountM;
};