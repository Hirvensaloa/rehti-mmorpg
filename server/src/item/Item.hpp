#pragma once

#include <string>

#include "RehtiReader.hpp"

class Entity;

class Item
{
public:
    Item(int id, std::string name, bool isStackable = false);

    Item(const GeneralItemStruct& generalItem);

    // Item type id, for example sword, bread etc.
    const int& getId();

    // Unique instance id for this item
    const int& getInstanceId();

    const unsigned int& getStackSize();

    const std::string& getName();

    const bool& IsStackable();

    virtual bool use(Entity* user); // If this returns true, item should be deleted.

protected:
    int idM;
    int instanceIdM;

    const unsigned int stackSizeM = 1; // TODO: Implement amount functionality for stackable items

    std::string nameM;

    bool isStackableM;

    inline static int nextInstanceIdM = 0;
};