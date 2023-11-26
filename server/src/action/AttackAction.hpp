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

    std::shared_ptr<Entity> pTargetM;

    std::chrono::milliseconds actionTimeM;

    std::chrono::milliseconds moveTimeM = MoveAction::getMoveTime();

    ActionType actionTypeM = ActionType::Attack;
};