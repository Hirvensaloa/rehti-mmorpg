#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(std::string name, unsigned int id, Coordinates location) : Entity(id, location), nameM(name){};

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