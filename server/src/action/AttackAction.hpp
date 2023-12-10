#pragma once

#include <optional>

#include "../world/Coordinates.hpp"
#include "Action.hpp"
#include "MoveAction.hpp"

class AttackAction : public Action
{
public:
    AttackAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> pTarget, std::shared_ptr<Entity> pEntity);

    ~AttackAction() = default;

    std::shared_ptr<Entity>& getTarget();

    void act();

    /**
     * @brief Returns information about the current action
     *
     * @return CurrentAction
     */
    CurrentAction getActionInfo();

private:
    std::optional<Coordinates> findNextMove();

    /**
     * @brief Finds a path to the target
     *
     * @return List of x,y pairs. Empty if no path is found
     */
    std::vector<std::pair<unsigned int, unsigned int>> findPathToTarget();

    std::shared_ptr<Entity> pTargetM;

    std::chrono::milliseconds actionTimeM;

    std::chrono::milliseconds moveTimeM = MoveAction::getMoveTime();

    ActionType actionTypeM = ActionType::Attack;

    std::vector<std::pair<unsigned int, unsigned int>> pathToTargetM; ///< Path to the target
    std::optional<Coordinates> nextMoveM;                             ///< The next move to be taken, is defined if the target is not in range
    bool targetInRangeM = true;                                       ///< True if the target is in range
};