#pragma once

#include <string>

class Entity;

class Item
{
public:
    Item(int id, std::string name, bool isStackable = false);

    int getId();

    std::string getName();

    bool IsStackable();

    virtual bool use(Entity *user) = 0; // If this returns true, item should be deleted.

protected:
    int idM;

    std::string nameM;

    bool isStackableM;
};