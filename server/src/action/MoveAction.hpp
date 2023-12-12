#pragma once

#include "../world/Coordinates.hpp"
#include "Action.hpp"

#include <vector>

class MoveAction : public Action
{
public:
    MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity);

    ~MoveAction() = default;

    Coordinates getTarget();

    void act();

    /**
     * @brief Returns information about the current action
     *
     * @return CurrentAction
     */
    CurrentAction getActionInfo();

private:
    Coordinates targetM;

    std::vector<std::pair<int, int>> pathM;
    std::optional<Coordinates> nextMoveM; ///< Next move to be taken. Stores the first element of pathM, after its deleted. Ww need this to be able to tell the client where we are moving next.

    std::chrono::milliseconds actionTimeM;

    ActionType actionTypeM = ActionType::Move;
};