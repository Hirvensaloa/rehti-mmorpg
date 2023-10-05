#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(std::string name, unsigned int id, Coordinates location) : Entity(id, location), nameM(name), currentActionM(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Coordinates(), this)){};

std::string PlayerCharacter::getName()
{
    return nameM;
}

Action &PlayerCharacter::getCurrentAction()
{
    return *currentActionM;
}

void PlayerCharacter::setAction(std::shared_ptr<Action> action)
{
    currentActionM = action;
}

void PlayerCharacter::move(Coordinates target)
{
    int xmov = locationM.x == target.x ? 0 : ((locationM.x - target.x) > 0 ? -1 : 1);
    int ymov = locationM.y == target.y ? 0 : ((locationM.y - target.y) > 0 ? -1 : 1);
    locationM.x += xmov;
    locationM.y += ymov;
    std::cout
        << "player: " << getName() << ", location: "
        << "x: " << locationM.x << ", y: " << locationM.y << ", z: " << locationM.z
        << std::endl;
}