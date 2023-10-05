#include "Action.hpp"
#include "PlayerCharacter.hpp"

#include <iostream>

Action::Action(std::chrono::system_clock::time_point startTime, PlayerCharacter *pPlayer) : startTimeM(startTime), pPlayerM(pPlayer) {}

std::chrono::system_clock::time_point Action::getStartTime()
{
    return startTimeM;
}

std::chrono::milliseconds Action::getActionTime()
{
    return actionTimeM;
}

Action::ActionType Action::getActionType()
{
    return actionTypeM;
}