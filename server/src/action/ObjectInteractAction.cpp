#include <iostream>

#include "../world/GameWorld.hpp"
#include "../world/Map.hpp"
#include "ObjectInteractAction.hpp"

ObjectInteractAction::ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Object> pTarget, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), pTargetM(pTarget), moveTimeM(pEntityM->getMoveSpeed()) {}

void ObjectInteractAction::act()
{
    if (completedM)
    {
        return;
    }

    if (!pTargetM->canInteract(*pEntityM))
    {
        completedM = true;
        return;
    }

    int currentDistance = pEntityM->getLocation().distance(pTargetM->getLocation());
    if (currentDistance <= pEntityM->getRange() && currentDistance > 0)
    {
        if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
        {
            reader::ObjectType type = pTargetM->getObjectType();
            // If item type is resource then we will continue the action forever, until stopped by the player
            if (type == reader::ObjectType::RESOURCE)
            {
                pTargetM->interact(*pEntityM);
                startTimeM = std::chrono::system_clock::now();
            }
            else
            {
                pTargetM->interact(*pEntityM);
                completedM = true;
            }
        }

        targetInRangeM = true;
    }
    else
    {
        targetInRangeM = false;

        // If there is no path to the target or the target has changed location, we will try to find one
        if (pathToTargetM.size() == 0 || (pathToTargetM.back().first != pTargetM->getLocation().x || pathToTargetM.back().second != pTargetM->getLocation().y))
        {
            pathToTargetM = pEntityM->getGameWorld()->getMap().findPathToRange(pEntityM->getLocation(), pTargetM->getLocation(), pEntityM->getRange());

            // If we cannot find a path to the target, we will just stop the action
            if (pathToTargetM.size() == 0)
            {
                nextMoveM = std::nullopt;
                completedM = true;
                return;
            }
            else
            {
                nextMoveM = Coordinates(pathToTargetM.front().first, pathToTargetM.front().second);
            }
        }

        // If the time since the last move is greater than the move time, we will move to the next location in the path. NOTE: We end up here only if the path is found.
        if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
        {
            auto next = pathToTargetM.front();
            nextMoveM = Coordinates(next.first, next.second);

            pEntityM->move(nextMoveM.value());

            pathToTargetM.erase(pathToTargetM.begin());
            startTimeM = std::chrono::system_clock::now();

            if (pathToTargetM.size() == 0)
            {
                nextMoveM = std::nullopt;
            }
            else
            {
                next = pathToTargetM.front();
                nextMoveM = Coordinates(next.first, next.second);
            }
        }
    }
}

CurrentAction ObjectInteractAction::getActionInfo()
{
    CurrentAction actionInfo;

    // If the target is not in range, we will return the next move to be taken
    if (!targetInRangeM)
    {
        if (nextMoveM.has_value())
        {
            actionInfo.id = ActionType::Move;
            actionInfo.durationMs = moveTimeM.count();
            actionInfo.looping = false;
            actionInfo.targetId = pTargetM->getId();
            const Coordinates nextMove = nextMoveM.value();
            actionInfo.targetCoordinate = {nextMove.x, nextMove.y, nextMove.z};
            return actionInfo;
        }
        else
        {
            actionInfo.id = ActionType::None;
            actionInfo.durationMs = 1000;
            actionInfo.looping = true;
            actionInfo.targetId = pTargetM->getId();
            Coordinates& coords = pEntityM->getLocation();
            actionInfo.targetCoordinate = {coords.x, coords.y, coords.z};
            return actionInfo;
        }
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = pTargetM->getId();
        const Coordinates& coords = pTargetM->getLocation();
        actionInfo.targetCoordinate = {coords.x,
                                       coords.y,
                                       coords.z};
        return actionInfo;
    }
}