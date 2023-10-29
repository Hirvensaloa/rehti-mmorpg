#pragma once

#include <optional>

#include "../world/Coordinates.hpp"
#include "Action.hpp"
#include "MoveAction.hpp"

class AttackAction : public Action
{
public:
    AttackAction(std::chrono::system_clock::time_point startTime, Entity *pTarget, Entity *pEntity);

    ~AttackAction() = default;

    Entity *getTarget();

    void act();

private:
    std::optional<Coordinates> findNextMove();

    Entity *pTargetM;

    std::chrono::milliseconds actionTimeM;

    std::chrono::milliseconds moveTimeM = MoveAction::getMoveTime();

    ActionType actionTypeM = ActionType::Attack;
};