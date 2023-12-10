#include <iostream>

#include "../world/GameWorld.hpp"
#include "../world/Map.hpp"
#include "ObjectInteractAction.hpp"

ObjectInteractAction::ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Object> pTarget, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), pTargetM(pTarget) {}

void ObjectInteractAction::act()
{
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
            nextMoveM = findNextMove();

            // If we cannot find a path to the target, we will just stop the action
            if (!nextMoveM.has_value())
            {
                completedM = true;
                return;
            };

            pEntityM->move(nextMoveM.value());
            startTimeM = std::chrono::system_clock::now();

            targetInRangeM = false;
        }
    }
}

std::optional<Coordinates> ObjectInteractAction::findNextMove()
{
    Coordinates pLocation = pEntityM->getLocation();
    Coordinates tLocation = pTargetM->getLocation();
    Map& map = pEntityM->getGameWorld()->getMap();
    if (!(pLocation == tLocation))
    {
        // Here we can take the path straight to the target location, because we will never actually move onto the target's location as we will be in range to interact
        auto path = map.findPath(pLocation, tLocation);

        if (path.empty())
        {
            return std::nullopt;
        }

        return Coordinates(path[0].first, path[0].second);
    }
    else // Find some available tile next to the target to move to, in case the target moved to our location. Just keeps this here, if for some reason the object could move.
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
                else
                {
                    return std::nullopt;
                }
            }
        }
        return pLocation;
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
            actionInfo.targetId = std::stoi(pTargetM->getInstanceId());
            actionInfo.targetCoordinate = {nextMoveM.value().x, nextMoveM.value().y, nextMoveM.value().z};
            return actionInfo;
        }
        else
        {
            actionInfo.id = ActionType::None;
            actionInfo.durationMs = 1000;
            actionInfo.looping = true;
            actionInfo.targetId = std::stoi(pTargetM->getInstanceId());
            return actionInfo;
        }
    }
    else
    {
        actionInfo.id = actionTypeM;
        actionInfo.durationMs = actionTimeM.count();
        actionInfo.looping = true;
        actionInfo.targetId = std::stoi(pTargetM->getInstanceId());
        return actionInfo;
    }
}