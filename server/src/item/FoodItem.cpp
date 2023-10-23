#include "FoodItem.hpp"
#include "../entity/Entity.hpp"

FoodItem::FoodItem(int id, std::string name, int healAmount, bool isStackable) : Item(id, name, isStackable), healAmountM(healAmount){};

int FoodItem::getHealAmount()
{
    return healAmountM;
}

bool FoodItem::use(Entity *user)
{
    user->changeHp(healAmountM);
    return true;
}