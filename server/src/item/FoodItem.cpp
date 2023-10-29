#include "FoodItem.hpp"
#include "../entity/Entity.hpp"

FoodItem::FoodItem(int id, std::string name, int healAmount, bool isStackable) : Item(id, name, isStackable), healAmountM(healAmount){};

FoodItem::FoodItem(const FoodItemStruct &foodItem) : Item(foodItem.id, foodItem.name, foodItem.isStackable), healAmountM(foodItem.healAmount){};

int FoodItem::getHealAmount()
{
    return healAmountM;
}

bool FoodItem::use(Entity *user)
{
    user->changeHp(healAmountM);
    return true;
}