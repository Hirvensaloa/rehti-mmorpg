#include "AttackAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

AttackAction::AttackAction(std::chrono::system_clock::time_point startTime, Entity *target, Entity *pEntity) : Action(startTime, pEntity), pTargetM(target), actionTimeM(pEntityM->getAttackSpeed()) {}

Entity *AttackAction::getTarget()
{
    return pTargetM;
}

void AttackAction::act()
{
    if (!completedM)
    {
        int currentDistance = pEntityM->getLocation().distance(pTargetM->getLocation());
        if (currentDistance <= pEntityM->getRange() && currentDistance > 0)
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
            pEntityM->move(findNextMove());
            startTimeM = std::chrono::system_clock::now();
        }
    }
}

Coordinates AttackAction::findNextMove()
{
    Coordinates pLocation = pEntityM->getLocation();
    Coordinates tLocation = pTargetM->getLocation();
    Map &map = pEntityM->getGameWorld()->getMap();
    if (!(pLocation == tLocation)) // Here we can take the path straight to the target location, because we will never actually move onto the target's location as we will be in range to attack
    {
        auto path = map.findPath(pLocation, tLocation);
        return Coordinates(path[0].first, path[0].second);
    }
    else // Find some available tile next to the target to move to, in case the target moved to our location
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                auto pathToNeighbor = map.findPath(pLocation, Coordinates(tLocation.x + i, tLocation.y + j));
                if (pathToNeighbor.size() != 0)
                {
                    return Coordinates(pathToNeighbor[0].first, pathToNeighbor[0].second);
                }
            }
        }
        return pLocation;
    }
}
