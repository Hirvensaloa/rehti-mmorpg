#pragma once

#include <string>

#include "Entity.hpp"
#include "MoveAction.hpp"

class PlayerCharacter : public Entity
{

public:
    PlayerCharacter(std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    std::string getName();

    Action &getCurrentAction();

    void setAction(std::shared_ptr<Action> action);

    void move(Coordinates target);

private:
    std::string nameM;
    std::shared_ptr<Action> currentActionM;
};