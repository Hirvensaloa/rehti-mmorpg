#include "Inventory.hpp"
#include "../entity/Entity.hpp"

Inventory::Inventory(Entity *owner, int inventorySize) : ownerM(owner), inventorySizeM(inventorySize){};

bool Inventory::addItem(std::shared_ptr<Item> item)
{
    if (itemsM.size() <= inventorySizeM)
    {
        itemsM.push_back(item);
        return true;
    }
    return false;
}

bool Inventory::removeItem(int itemId)
{
    for (auto it = itemsM.begin(); it != itemsM.end(); it++)
    {
        if ((*it)->getId() == itemId)
        {
            itemsM.erase(it);
            return true;
        }
    }
    return false;
}

const std::vector<std::shared_ptr<Item>> &Inventory::getItems() const
{
    return itemsM;
}

void Inventory::useItem(int itemId)
{
    for (auto it = itemsM.begin(); it != itemsM.end(); it++)
    {
        if ((*it)->getId() == itemId)
        {
            bool remove = (*it)->use(ownerM);
            if (remove)
            {
                itemsM.erase(it);
            }
            return;
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Inventory &inv)
{
    os << "Inventory contents:" << std::endl;
    for (auto item : inv.getItems())
    {
        os << "     Item Id: " << item->getId() << ", Name: " << item->getName() << std::endl;
    }
    return os;
}
