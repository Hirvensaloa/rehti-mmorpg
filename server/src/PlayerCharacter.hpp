#pragma once

#include "Entity.hpp"
#include <string>

class PlayerCharacter : public Entity
{

public:
    PlayerCharacter(std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    std::string getName();

private:
    std::string nameM;
};