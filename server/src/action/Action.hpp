#pragma once

#include <chrono>
#include <ctime>
#include <memory>

#include "network.h"

class Entity;

/**
 * @brief Represents an action that an entity can perform
 */
class Action
{
public:
    Action(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> pEntity);

    ~Action() = default;

    /**
     * @brief Get the time when the action started
     *
     * @return std::chrono::system_clock::time_point
     */
    std::chrono::system_clock::time_point getStartTime();

    /**
     * @brief Get the time how long a single iteration of the action takes
     *
     * @return std::chrono::milliseconds
     */
    std::chrono::milliseconds getActionTime();

    /**
     * @brief Get the ActionType object
     *
     * @return ActionType
     */
    ActionType getActionType();

    /**
     * @brief Returns information about the current action. This is used to send information to the client.
     *
     * @return CurrentAction
     */
    virtual CurrentAction getActionInfo();

    /**
     * @brief Returns true if the action is completed
     *
     * @return boolean
     */
    bool isCompleted();

    /**
     * @brief Functionality for the action, pure virtual function that needs to be implemented by derived classes.
     * Gets called on every server tick.
     */
    virtual void act() = 0;

protected:
    std::chrono::system_clock::time_point startTimeM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(0);

    ActionType actionTypeM = ActionType::None;

    std::shared_ptr<Entity> pEntityM;

    bool completedM = false;
};
