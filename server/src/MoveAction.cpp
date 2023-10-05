#include "MoveAction.hpp"
#include "PlayerCharacter.hpp"

#include <iostream>

MoveAction::MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, PlayerCharacter *pPlayer) : Action(startTime, pPlayer), targetM(target) {}

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
            pPlayerM->move(targetM);
            startTimeM = std::chrono::system_clock::now();
            if (pPlayerM->getLocation() == targetM)
            {
                completedM = true;
            }
        }
    }
}