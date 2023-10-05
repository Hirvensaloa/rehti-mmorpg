#include "PlayerCharacter.hpp"

PlayerCharacter::PlayerCharacter(std::string name, unsigned int id, Coordinates location) : Entity(id, location), nameM(name){};

std::string PlayerCharacter::getName()
{
    return nameM;
}