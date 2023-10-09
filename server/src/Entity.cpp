#include <iostream>

#include "Entity.hpp"

Entity::Entity(unsigned int id, Coordinates location) : idM(id), locationM(location){};

unsigned int Entity::getId()
{
    return idM;
}

Coordinates Entity::getLocation()
{
    return locationM;
}

Action &Entity::getCurrentAction()
{
    return *currentActionM;
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