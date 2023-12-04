#pragma once

#include <string>

#include "../skill/SkillSet.hpp"
#include "Entity.hpp"

class PlayerCharacter : public Entity
{
public:
    PlayerCharacter(GameWorld* pGameWorld, std::string name, int baseDamage, int baseAccuracy, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    void update();

    void respawn();
};