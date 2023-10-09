#include "MoveAction.hpp"
#include "PlayerCharacter.hpp"

#include <iostream>

MoveAction::MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, Entity *pEntity) : Action(startTime, pEntity), targetM(target) {}

Coordinates MoveAction::getTarget()
{
    return targetM;
}

void MoveAction::act()
{
    if (!completedM)
    {
        if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
        {
            pEntityM->move(targetM);
            startTimeM = std::chrono::system_clock::now();
            if (pEntityM->getLocation() == targetM)
            {
                completedM = true;
            }
        }
    }
}