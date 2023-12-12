#include <iostream>

#include "../world/GameWorld.hpp"
#include "../world/Map.hpp"
#include "ObjectInteractAction.hpp"

ObjectInteractAction::ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Object> pTarget, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), pTargetM(pTarget) {}

void ObjectInteractAction::act()
{
    if (!pTargetM->canInteract(*pEntityM))
    {
        completedM = true;
        return;
    }

    if (!completedM)
    {
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
        else if (std::chrono::system_clock::now() > startTimeM + moveTimeM)
        {
            targetInRangeM = false;

            // If there is no path to the target or the target has changed location, we will try to find one
            if (pathToTargetM.size() == 0 || (pathToTargetM.back().first != pTargetM->getLocation().x || pathToTargetM.back().second != pTargetM->getLocation().y))
            {
                pathToTargetM = pEntityM->getGameWorld()->getMap().findPath(pEntityM->getLocation(), pTargetM->getLocation());

                // If we cannot find a path to the target, we will just stop the action
                if (pathToTargetM.size() == 0)
                {
                    nextMoveM = std::nullopt;
                    completedM = true;
                    return;
                };
            }

            auto next = pathToTargetM.front();
            nextMoveM = Coordinates(next.first, next.second);

            pEntityM->move(nextMoveM.value());

            pathToTargetM.erase(pathToTargetM.begin());
            startTimeM = std::chrono::system_clock::now();
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
            return actionInfo;
        }
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = pTargetM->getId();
        return actionInfo;
    }
}