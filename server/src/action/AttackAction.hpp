#pragma once

#include "../world/Coordinates.hpp"
#include "Action.hpp"

class AttackAction : public Action
{
public:
    AttackAction(std::chrono::system_clock::time_point startTime, Entity *pTarget, Entity *pEntity);

    ~AttackAction() = default;

    Entity *getTarget();

    void act();

private:
    Entity *pTargetM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(300);

    std::chrono::milliseconds moveTimeM = std::chrono::milliseconds(200);

    ActionType actionTypeM = ActionType::Attack;
};