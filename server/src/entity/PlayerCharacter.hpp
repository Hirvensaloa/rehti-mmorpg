#pragma once

#include <string>

#include "Entity.hpp"

class PlayerCharacter : public Entity
{

public:
    PlayerCharacter(GameWorld *pGameWorld, std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    void update();
};