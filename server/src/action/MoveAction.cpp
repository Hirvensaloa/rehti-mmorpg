#include "MoveAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

MoveAction::MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), targetM(target)
{
    pathM = pEntity->getGameWorld()->getMap().findPath(pEntityM->getLocation(), targetM);
    if (pathM.size() > 0)
    {
        auto next = pathM.front();
        nextMoveM = Coordinates(next.first, next.second);
    }
}

Coordinates MoveAction::getTarget()
{
    return targetM;
}

void MoveAction::act()
{
    if (completedM)
    {
        return;
    }

    if (pathM.size() == 0)
    {
        completedM = true;
    }

    if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
    {
        auto next = pathM.front();
        nextMoveM = Coordinates(next.first, next.second);
        if (pEntityM->move(nextMoveM.value()))
        {
            pathM.erase(pathM.begin());
        }

        else
        {
            std::cout << "illegal move" << std::endl;
        }
        startTimeM = std::chrono::system_clock::now();
        if (pEntityM->getLocation() == targetM)
        {
            completedM = true;
        }
    }
}

CurrentAction MoveAction::getActionInfo()
{
    CurrentAction actionInfo;
    actionInfo.id = actionTypeM;
    actionInfo.durationMs = actionTimeM.count();
    actionInfo.looping = true;

    if (nextMoveM.has_value())
    {
        const Coordinates nextTarget = nextMoveM.value();
        actionInfo.targetCoordinate = {nextTarget.x, nextTarget.y, nextTarget.z};
    }
    else
    {
        const Coordinates target = pEntityM->getLocation();
        actionInfo.targetCoordinate = {target.x, target.y, target.z};
    }

    return actionInfo;
}
