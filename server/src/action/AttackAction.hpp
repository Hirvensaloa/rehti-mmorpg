#pragma once

#include <optional>

#include "../world/Coordinates.hpp"
#include "Action.hpp"
#include "MoveAction.hpp"

/**
 * @brief Action used for attacks
 */
class AttackAction : public Action
{
public:
    AttackAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> pTarget, std::shared_ptr<Entity> pEntity);

    ~AttackAction() = default;

    /**
     * @brief Get the target Entity of the action
     *
     * @return std::shared_ptr<Entity>
     */
    std::shared_ptr<Entity>& getTarget();

    /**
     * @brief Attack functionality, which performs attacks against target and moves closer to the target if not in range.
     * Implementation of pure virtual method act() of the base class.
     */
    void act();

    /**
     * @brief Returns information about the current action
     *
     * @return CurrentAction
     */
    CurrentAction getActionInfo();

private:
    /**
     * @brief Finds a path to the target
     *
     * @return List of x,y pairs. Empty if no path is found
     */
    std::vector<std::pair<int, int>> findPathToTarget();

    std::shared_ptr<Entity> pTargetM;

    std::chrono::milliseconds actionTimeM;

    std::chrono::milliseconds moveTimeM;

    ActionType actionTypeM = ActionType::Attack;

    std::vector<std::pair<int, int>> pathToTargetM; ///< Path to the target
    std::optional<Coordinates> nextMoveM;           ///< The next move to be taken, is defined if the target is not in range
    bool targetInRangeM = false;                    ///< True if the target is in range
};