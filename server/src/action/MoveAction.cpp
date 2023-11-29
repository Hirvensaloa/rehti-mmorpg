#include "MoveAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

MoveAction::MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), targetM(target), pathM(pEntity->getGameWorld()->getMap().findPath(pEntityM->getLocation(), targetM)) {}

Coordinates MoveAction::getTarget()
{
    return targetM;
}

void MoveAction::act()
{
    if (pathM.size() == 0)
    {
        completedM = true;
    }

    if (!completedM)
    {
        if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
        {
            auto next = pathM.front();
            if (pEntityM->move(Coordinates(next.first, next.second)))
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
}

const std::chrono::milliseconds MoveAction::getMoveTime()
{
    return std::chrono::milliseconds(200);
}

CurrentAction MoveAction::getActionInfo()
{
    CurrentAction actionInfo;
    actionInfo.id = actionTypeM;
    actionInfo.durationMs = actionTimeM.count();
    actionInfo.looping = true;

    if (pathM.size())
    {
        int x = static_cast<int>(pathM.front().first);
        int y = static_cast<int>(pathM.front().second);
        const Coordinates nextTarget = {x, y};
        actionInfo.targetCoordinate = {nextTarget.x, nextTarget.y, nextTarget.z};
    }

    return actionInfo;
}
