#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : Entity(pGameWorld, name, id, location)
{
    respawnLocationM = Coordinates(5, 5);
    respawnTimeM = std::chrono::milliseconds(2000);
};

void PlayerCharacter::update()
{

    if (currentActionM != nullptr)
    {
        currentActionM->act();
    }
}

void PlayerCharacter::respawn()
{
    getInventory().removeAllItems(); // TODO: Drop the items on the ground, as well as equipment
    setAction(std::make_shared<RespawnAction>(std::chrono::system_clock::now(), respawnTimeM, this->shared_from_this()));
}