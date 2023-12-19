#include "AttackAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

AttackAction::AttackAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> target, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), pTargetM(target), actionTimeM(pEntityM->getAttackSpeed()), moveTimeM(pEntityM->getMoveSpeed())
{
}

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
        else
        {
            if (targetInRangeM)
            {
                startTimeM = std::chrono::system_clock::now();
            }
            targetInRangeM = false;

            if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
            {

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
}

std::vector<std::pair<int, int>> AttackAction::findPathToTarget()
{
    Coordinates pLocation = pEntityM->getLocation();
    Coordinates tLocation = pTargetM->getLocation();
    Map& map = pEntityM->getGameWorld()->getMap();

    return map.findPathToRange(pLocation, tLocation, pEntityM->getRange());
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

        // Even though we are moving, we still want to inform the other entities about the target we are trying to attack
        actionInfo.targetId = pTargetM->getInstanceId();

        return actionInfo;
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = pTargetM->getInstanceId();
        Coordinates& coords = pTargetM->getLocation();
        actionInfo.targetCoordinate = {coords.x, coords.y, coords.z};
        return actionInfo;
    }
}