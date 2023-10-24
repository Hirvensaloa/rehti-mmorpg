#include "Item.hpp"

Item::Item(int id, std::string name, bool isStackable) : idM(id), nameM(name), isStackableM(isStackable){};

int Item::getId()
{
    return idM;
}

std::string Item::getName()
{
    return nameM;
}

bool Item::IsStackable()
{
    return isStackableM;
}