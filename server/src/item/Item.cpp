#include "Item.hpp"

Item::Item(int id, std::string name, bool isStackable) : idM(id), nameM(name), isStackableM(isStackable), instanceIdM(nextInstanceIdM++){};

Item::Item(const GeneralItemStruct &generalItem) : idM(generalItem.id), nameM(generalItem.name), isStackableM(generalItem.isStackable), instanceIdM(nextInstanceIdM++){};

const int &Item::getId()
{
    return idM;
}

const std::string &Item::getName()
{
    return nameM;
}

const bool &Item::IsStackable()
{
    return isStackableM;
}

bool Item::use(Entity *user)
{
    std::cout << "This item is not usable" << std::endl;
    return false;
};

const int &Item::getInstanceId()
{
    return instanceIdM;
}

const unsigned int &Item::getStackSize()
{
    return stackSizeM;
}