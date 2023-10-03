#pragma once

#include "Coordinates.hpp"

class Entity
{
public:
    Entity(unsigned int id = 0, Coordinates location = Coordinates());

    ~Entity() = default;

    unsigned int getId();

    Coordinates getLocation();

private:
    unsigned int idM;
    Coordinates locationM;
};