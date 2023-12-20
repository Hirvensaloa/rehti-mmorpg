#include "Inventory.hpp"
#include "../entity/Entity.hpp"

Inventory::Inventory(Entity* owner) : ownerM(owner){};

bool Inventory::addItem(std::shared_ptr<Item> item)
{
    if (itemsM.size() < inventorySizeM)
    {
        itemsM.push_back(item);
        return true;
    }
    return false;
}

std::shared_ptr<Item> Inventory::removeItem(int itemId)
{
    for (auto it = itemsM.begin(); it != itemsM.end(); it++)
    {
        if ((*it)->getInstanceId() == itemId)
        {
            std::shared_ptr<Item> removedItem = std::move(*it);
            itemsM.erase(it);
            return removedItem;
        }
    }
    return std::shared_ptr<Item>();
}

void Inventory::removeAllItems()
{
    itemsM.clear();
}

bool Inventory::isFull() const
{
    return itemsM.size() == inventorySizeM;
}

const std::vector<std::shared_ptr<Item>>& Inventory::getItems() const
{
    return itemsM;
}

void Inventory::useItem(int itemId)
{
    for (auto it = itemsM.begin(); it != itemsM.end(); it++)
    {
        if ((*it)->getInstanceId() == itemId)
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

std::ostream& operator<<(std::ostream& os, const Inventory& inv)
{
    os << "Inventory contents:" << std::endl;
    for (auto item : inv.getItems())
    {
        os << "     Item Id: " << item->getInstanceId() << ", Name: " << item->getName() << std::endl;
    }
    return os;
}
