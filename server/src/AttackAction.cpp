#include "AttackAction.hpp"
#include "PlayerCharacter.hpp"

#include <iostream>

AttackAction::AttackAction(std::chrono::system_clock::time_point startTime, Entity *target, Entity *pEntity) : Action(startTime, pEntity), pTargetM(target) {}

Entity *AttackAction::getTarget()
{
    return pTargetM;
}

void AttackAction::act()
{
    if (!completedM)
    {
        if (pEntityM->getLocation().distance(pTargetM->getLocation()) <= 1)
        {
            if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
            {
                pEntityM->attack(*pTargetM);
                startTimeM = std::chrono::system_clock::now();
                if (pTargetM->getHp() == 0)
                {
                    std::cout << "Attack action completed" << std::endl;
                    completedM = true;
                }
            }
        }

        else if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
        {
            pEntityM->move(pTargetM->getLocation());
            startTimeM = std::chrono::system_clock::now();
        }
    }
}
