#include "Action.hpp"
#include "../entity/PlayerCharacter.hpp"

#include <iostream>

Action::Action(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> pEntity) : startTimeM(startTime), pEntityM(pEntity) {}

std::chrono::system_clock::time_point Action::getStartTime()
{
    return startTimeM;
}

std::chrono::milliseconds Action::getActionTime()
{
    return actionTimeM;
}

ActionType Action::getActionType()
{
    return actionTypeM;
}

CurrentAction Action::getActionInfo()
{
    CurrentAction actionInfo;
    actionInfo.id = actionTypeM;
    actionInfo.durationMs = actionTimeM.count();
    actionInfo.looping = false;
    actionInfo.targetId = pEntityM->getInstanceId();
    return actionInfo;
};

bool Action::isCompleted()
{
    return completedM;
}