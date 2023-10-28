#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(std::string name, AssetManager &assetManager, unsigned int id, Coordinates location) : Entity(id, location), nameM(name), skillSetM(SkillSet(assetManager)){};

std::string PlayerCharacter::getName()
{
    return nameM;
}

void PlayerCharacter::update()
{
    if (currentActionM)
    {
        currentActionM->act();
    }
}

SkillSet PlayerCharacter::getSkillSet()
{
    return skillSetM;
}