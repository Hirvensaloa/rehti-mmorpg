#pragma once

#include "Action.hpp"
#include "../world/Coordinates.hpp"

class MoveAction : public Action
{
public:
    MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, Entity *pEntity);

    ~MoveAction() = default;

    Coordinates getTarget();

    void act();

private:
    Coordinates targetM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(200);

    ActionType actionTypeM = ActionType::Move;
};