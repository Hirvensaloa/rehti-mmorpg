#include "RespawnAction.hpp"

#include "../entity/Entity.hpp"

#include <iostream>

RespawnAction::RespawnAction(std::chrono::system_clock::time_point startTime, std::chrono::milliseconds respawnTime, std::shared_ptr<Entity> pEntity) : Action(startTime, pEntity), actionTimeM(respawnTime) {}

void RespawnAction::act()
{
    if (completedM)
    {
        return;
    }

    if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
    {
        pEntityM->changeHp(pEntityM->getMaxHp());
        pEntityM->setLocation(pEntityM->getRespawnLocation());
        completedM = true;
        std::cout << "Entity " << pEntityM->getName() << "Respawned" << std::endl;
    }
}

CurrentAction RespawnAction::getActionInfo()
{
    CurrentAction actionInfo;
    actionInfo.id = actionTypeM;
    actionInfo.durationMs = actionTimeM.count();
    actionInfo.looping = true;
    actionInfo.targetId = pEntityM->getInstanceId();
    Coordinates& coords = pEntityM->getLocation();
    actionInfo.targetCoordinate = {coords.x, coords.y, coords.z};
    return actionInfo;
}