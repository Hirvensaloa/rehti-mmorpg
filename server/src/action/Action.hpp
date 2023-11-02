#pragma once

#include <chrono>
#include <ctime>
#include <memory>

#include "network.h"

class Entity;

class Action
{
public:
    enum ActionType
    {
        None = -1,
        Move = MessageId::Move,
        Attack = MessageId::Attack,
        ObjectInteract = MessageId::ObjectInteract,
    };

    Action(std::chrono::system_clock::time_point startTime, std::shared_ptr<Entity> pEntity);

    ~Action() = default;

    std::chrono::system_clock::time_point getStartTime();

    std::chrono::milliseconds getActionTime();

    ActionType getActionType();

    bool isCompleted();

    virtual void act() = 0;

protected:
    std::chrono::system_clock::time_point startTimeM;

    std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(0);

    ActionType actionTypeM = ActionType::None;

    std::shared_ptr<Entity> pEntityM;

    bool completedM = false;
};
