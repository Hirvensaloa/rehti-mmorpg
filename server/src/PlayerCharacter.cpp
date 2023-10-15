#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : Entity(pGameWorld, name, id, location){};

void PlayerCharacter::update()
{
    if (currentActionM)
    {
        currentActionM->act();
    }
}