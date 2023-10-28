#pragma once

#include <string>

#include "Entity.hpp"
#include "../skill/SkillSet.hpp"

class PlayerCharacter : public Entity
{
public:
    PlayerCharacter(GameWorld *pGameWorld, std::string name, AssetManager &assetManager, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    void update();

    SkillSet getSkillSet();

private:
    std::string nameM;
    SkillSet skillSetM;
};