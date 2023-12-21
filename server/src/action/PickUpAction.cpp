#include "PickUpAction.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

PickUpAction::PickUpAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity, int targetItemId)
    : MoveAction(startTime, target, pEntity), targetItemIdM(targetItemId) {}

void PickUpAction::act()
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
        if (pEntityM->move(Coordinates(next.first, next.second)))
        {
            pathM.erase(pathM.begin());
        }

        startTimeM = std::chrono::system_clock::now();
        if (pEntityM->getLocation() == targetM)
        {
            pEntityM->pickUpItem(targetItemIdM, targetM);
            completedM = true;
        }
        else
        {
            auto next = pathM.front();
            nextMoveM = Coordinates(next.first, next.second);
        }
    }
}