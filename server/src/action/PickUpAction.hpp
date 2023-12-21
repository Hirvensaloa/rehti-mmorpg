#pragma once

#include "MoveAction.hpp"

/**
 * @brief Special case of MoveAction where, at the end, the actor picks up an item
 */
class PickUpAction : public MoveAction
{
public:
    PickUpAction(std::chrono::system_clock::time_point startTime, Coordinates target, std::shared_ptr<Entity> pEntity, int targetItemId);

    ~PickUpAction() = default;

    void act();

private:
    int targetItemIdM;
};