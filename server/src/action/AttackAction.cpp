#include "AttackAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

AttackAction::AttackAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> target, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), pTargetM(target), actionTimeM(pEntityM->getAttackSpeed()) {}

std::shared_ptr<Entity>& AttackAction::getTarget()
{
    return pTargetM;
}

void AttackAction::act()
{
    if (pTargetM->isDisconnected())
    {
        completedM = true;
    }

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
                    std::cout << "Attack action completed. Target eliminated." << std::endl;
                    completedM = true;
                }
            }

            targetInRangeM = true;
        }

        else if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
        {
            nextMoveM = findNextMove();

            // If we cannot find a path to the target, do nothing e.g. stay agressive and wait for the target to potentially come to range.
            if (nextMoveM.has_value())
            {
                pEntityM->move(nextMoveM.value());
                startTimeM = std::chrono::system_clock::now();
            }

            targetInRangeM = false;
        }
    }
}

std::optional<Coordinates> AttackAction::findNextMove()
{
    Coordinates pLocation = pEntityM->getLocation();
    Coordinates tLocation = pTargetM->getLocation();
    Map& map = pEntityM->getGameWorld()->getMap();
    if (!(pLocation == tLocation)) // Here we can take the path straight to the target location, because we will never actually move onto the target's location as we will be in range to attack
    {
        auto path = map.findPath(pLocation, tLocation);

        if (path.empty())
        {
            return std::nullopt;
        }

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

CurrentAction AttackAction::getActionInfo()
{
    CurrentAction actionInfo;

    // If the target is not in range, we need display move action or none if we cannot find a path to the target
    if (!targetInRangeM)
    {
        if (nextMoveM.has_value())
        {
            actionInfo.id = ActionType::Move;
            actionInfo.durationMs = moveTimeM.count();
            actionInfo.looping = true;
            actionInfo.targetCoordinate = {nextMoveM.value().x, nextMoveM.value().y, nextMoveM.value().z};
        }
        else
        {
            actionInfo.id = ActionType::None;
            actionInfo.durationMs = 1000;
            actionInfo.looping = true;
            actionInfo.targetCoordinate = {pEntityM->getLocation().x, pEntityM->getLocation().y, pEntityM->getLocation().z};
        }
        return actionInfo;
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = pTargetM->getInstanceId();
        return actionInfo;
    }
}