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
                if (pTargetM->getHp() <= 0)
                {
                    std::cout << "Attack action completed. Target eliminated." << std::endl;
                    completedM = true;
                }
            }

            targetInRangeM = true;
        }

        else if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
        {
            targetInRangeM = false;

            // If the path to the target is empty or the target has changed location, we will try to find a new path
            if (pathToTargetM.size() == 0 || (pathToTargetM.back().first != pTargetM->getLocation().x || pathToTargetM.back().second != pTargetM->getLocation().y))
            {
                pathToTargetM = findPathToTarget();

                // If we cannot find a path to the target, just set the next move to none and do nothing. We want to stay aggressive and see if we can find a path later.
                if (pathToTargetM.size() == 0)
                {
                    nextMoveM = std::nullopt;
                    return;
                }
            }

            nextMoveM = Coordinates(pathToTargetM.front().first, pathToTargetM.front().second);

            pEntityM->move(nextMoveM.value());
            startTimeM = std::chrono::system_clock::now();
            pathToTargetM.erase(pathToTargetM.begin());
        }
    }
}

std::vector<std::pair<int, int>> AttackAction::findPathToTarget()
{
    // If we are on the same tile as the target, we will try to find a path to a neighbor tile
    if (pEntityM->getLocation() == pTargetM->getLocation())
    {
        Coordinates pLocation = pEntityM->getLocation();
        Coordinates tLocation = pTargetM->getLocation();
        Map& map = pEntityM->getGameWorld()->getMap();
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {

                auto pathToNeighbor = map.findPath(pLocation, Coordinates(tLocation.x + i, tLocation.y + j));
                if (pathToNeighbor.size() != 0)
                {
                    return pathToNeighbor;
                }
            }
        }
        return {};
    }
    else
    {
        return pEntityM->getGameWorld()->getMap().findPath(pEntityM->getLocation(), pTargetM->getLocation());
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
            Coordinates& coords = pEntityM->getLocation();
            actionInfo.targetCoordinate = {coords.x, coords.y, coords.z};
        }
        return actionInfo;
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = pTargetM->getInstanceId();
        Coordinates& coords = pEntityM->getLocation();
        actionInfo.targetCoordinate = {coords.x, coords.y, coords.z};
        return actionInfo;
    }
}