#pragma once

#include "Coordinates.hpp"
#include "MoveAction.hpp"

class Entity
{
public:
    Entity(unsigned int id = 0, Coordinates location = Coordinates());

    ~Entity() = default;

    unsigned int getId();

    Coordinates getLocation();

    Action &getCurrentAction();

    void setAction(std::shared_ptr<Action> action);

    void move(Coordinates location);

    virtual void update() = 0;

protected:
    unsigned int idM;
    Coordinates locationM;
    std::shared_ptr<Action> currentActionM;
};