#pragma once

#include <chrono>
#include <ctime>
#include <memory>

class Entity;

class Action
{
public:
    enum ActionType
    {
        None,
        Move,
        Attack,
    };

    Action(std::chrono::system_clock::time_point startTime, Entity *pEntity);

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

    Entity *pEntityM;

    bool completedM = false;
};
