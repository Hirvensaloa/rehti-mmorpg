#pragma once

#include "../world/Coordinates.hpp"
#include "Action.hpp"

#include <vector>

/**
 * @brief Action used for moving
 */
class MoveAction : public Action
{
public:
    MoveAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity);

    ~MoveAction() = default;

    /**
     * @brief Returns the target location of the action
     * @return Coordinates
     */
    Coordinates getTarget();

    /**
     * @brief Movement functionality, which finds the shortest path to target location and performs the movement there.
     * Implementation of pure virtual method act() of the base class
     */
    void act();

    /**
     * @brief Get the Move Time object
     *
     * @return const std::chrono::milliseconds
     */
    static const std::chrono::milliseconds getMoveTime();

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