#include "TimerCallbackSystem.hpp"

void TimerCallbackSystem::elapseTime(float dt)
{
    for (auto it = timersM.begin(); it != timersM.end();)
    {
        auto& timer = it->second;
        float deltaTime = dt;
        float elapsedTime = std::min(timer.time, deltaTime);
        timer.callback(elapsedTime);
        timer.time -= elapsedTime * timer.factor;
        if (abs(timer.time) < EPSILON)
        {
            timersM.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void TimerCallbackSystem::addTimerCallback(int id, float time, std::function<void(float dt)> callback, float factor)
{
    timersM[id] = {time, factor, callback};
}

bool TimerCallbackSystem::forceQuitCallback(int id)
{
    size_t elementsRemoved = timersM.erase(id);
    return 0 < elementsRemoved;
}

bool TimerCallbackSystem::finishCallback(int id)
{
    auto it = timersM.find(id);
    if (it != timersM.end())
    {
        // call the callback with the remaining time
        it->second.callback(it->second.time);
        timersM.erase(it);
        return true;
    }
    return false;
}
