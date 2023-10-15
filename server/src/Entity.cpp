#include <iostream>

#include "Entity.hpp"
#include "GameWorld.hpp"

Entity::Entity(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : idM(id), nameM(name), locationM(location), pGameWorldM(pGameWorld){};

unsigned int Entity::getId()
{
    return idM;
}

std::string Entity::getName()
{
    return nameM;
}

Coordinates &Entity::getLocation()
{
    return locationM;
}

Action &Entity::getCurrentAction()
{
    return *currentActionM;
}

unsigned int Entity::getHp()
{
    return hpM;
}

void Entity::changeHp(int amount)
{
    if (hpM > -amount)
    {
        hpM += amount;
    }
    else
    {
        hpM = 0;
    }
}

void Entity::setAction(std::shared_ptr<Action> action)
{
    currentActionM = action;
}

void Entity::move(Coordinates target)
{
    int xmov = locationM.x == target.x ? 0 : ((locationM.x - target.x) > 0 ? -1 : 1);
    int ymov = locationM.y == target.y ? 0 : ((locationM.y - target.y) > 0 ? -1 : 1);
    locationM.x += xmov;
    locationM.y += ymov;
}

void Entity::attack(Entity &target)
{
    unsigned int damage = 1;
    target.changeHp(-damage);
    std::cout << "Entity " << target.getName() << " took " << damage << " damage. Remaining HP: " << target.getHp() << std::endl;
}