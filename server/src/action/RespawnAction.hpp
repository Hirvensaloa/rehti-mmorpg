#pragma once

#include "Action.hpp"

/**
 * @brief Action used for respawning entities
 */
class RespawnAction : public Action
{
public:
    RespawnAction(std::chrono::system_clock::time_point startTime, std::chrono::milliseconds respawnTime, std::shared_ptr<Entity> pEntity);

    ~RespawnAction() = default;

    /**
     * @brief After time indicated by actionTimeM has passed, moves pEntity to its respawn location and changes hp back to maximum.
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
    std::chrono::milliseconds actionTimeM;

    ActionType actionTypeM = ActionType::Respawn;
};