#pragma once

#include "../object/Object.hpp"
#include "../world/Coordinates.hpp"
#include "Action.hpp"
#include "MoveAction.hpp"

/**
 * @brief Actions used for interacting with objects
 */
class ObjectInteractAction : public Action
{
public:
    ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Object> pTarget, std::shared_ptr<Entity> pEntity);

    ~ObjectInteractAction() = default;

    /**
     * @brief Objecct interact functionality, which performs target object's interact functionality, and moves entity next to the object if not already.
     * Implementation of pure virtual method act() of the base class
     */
    void act();

    /**
     * @brief Returns information about the current action
     *
     * @return CurrentAction
     */
    CurrentAction getActionInfo();

private:
    std::shared_ptr<Object> pTargetM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(3000);
    std::chrono::milliseconds moveTimeM;

    const ActionType actionTypeM = ActionType::ObjectInteract;

    std::vector<std::pair<int, int>> pathToTargetM; ///< Path to the target
    std::optional<Coordinates> nextMoveM;           ///< Next move to be taken
    bool targetInRangeM = true;                     ///< True if the target is in range
};