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