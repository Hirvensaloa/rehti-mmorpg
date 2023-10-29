#pragma once

#include "../world/Coordinates.hpp"
#include "Action.hpp"

#include <vector>

class MoveAction : public Action
{
public:
    MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, Entity *pEntity);

    ~MoveAction() = default;

    Coordinates getTarget();

    void act();

    static const std::chrono::milliseconds getMoveTime();

private:
    Coordinates targetM;

    std::vector<std::pair<unsigned int, unsigned int>> pathM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(200);

    ActionType actionTypeM = ActionType::Move;
};