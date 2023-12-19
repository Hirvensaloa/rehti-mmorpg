#include "TimerCallbackSystem.hpp"

void TimerCallbackSystem::elapseTime(float dt)
{
    std::unique_lock lock(mutexM);
    for (auto it = timersM.begin(); it != timersM.end();)
    {
        auto& timer = it->second;
        float deltaTime = dt;
        float elapsedTime = std::min(timer.time, deltaTime);
        timer.callback(elapsedTime);
        timer.time -= elapsedTime * timer.factor;
        if (std::abs(timer.time) < EPSILON)
        {
            // erase moves the iterator to the next element
            it = timersM.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void TimerCallbackSystem::addTimerCallback(int id, float time, std::function<void(float dt)> callback, float factor)
{
    std::unique_lock lock(mutexM);
    timersM[id] = {time, factor, callback};
}

bool TimerCallbackSystem::forceQuitCallback(int id)
{
    std::unique_lock lock(mutexM);
    size_t elementsRemoved = timersM.erase(id);
    return 0 < elementsRemoved;
}

bool TimerCallbackSystem::finishCallback(int id)
{
    std::unique_lock lock(mutexM);
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
