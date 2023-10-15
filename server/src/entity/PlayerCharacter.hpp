#pragma once

#include <string>

#include "Entity.hpp"

class PlayerCharacter : public Entity
{

public:
    PlayerCharacter(std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    std::string getName();

    void update();

private:
    std::string nameM;
};