#pragma once
#include <functional>
#include <map>
#include <mutex>

constexpr float EPSILON = 1e-6f;

/**
 * @brief Struct representing a callback with a timer. Timer is decremented with the given deltatime.
 */
struct CallBackTimer
{
    float time;   // time in seconds
    float factor; // factor to multiply the delta time with
    std::function<void(float dt)> callback;
};

/**
 * @brief Class for handling smooth interpolation of events. A callback can be registered with a given id and time. The callback will be called with the given delta time until the time is depleted.
 */
class TimerCallbackSystem
{
public:
    /**
     * @brief Elapses time by the given delta time, meaning calling the callbacks with the given delta time.
     * @param dt is the delta time to elapse in seconds.
     */
    void elapseTime(float dt);

    /**
     * @brief Adds a new timer callback for the given id
     * @param id is the identification for the given callback
     * @param time is the time it takes to finish the action set to the callback in seconds
     * @param callback is the function that can be called with given delta time
     */
    void addTimerCallback(int id, float time, std::function<void(float dt)> callback, float factor = 1.f);

    /**
     * @brief Immediately forces the removal of the callback with the given id, without finishing the remaining time
     * @param id to remove the callback
     * @returns true if the callback was removed, false, if the given id was not found
     */
    bool forceQuitCallback(int id);

    /**
     * @brief Forces the callback to be completed with the time left in the timer and removes it from the system
     * @param id of the object to be finished
     * @return true if the callback was found and finished, false if the callback was not found
     */
    bool finishCallback(int id);

private:
    // TODO use a custom data structure with the following properties:
    // 1. Fast iteration
    // 2. Fast removal while iterating
    // 3. Semi fast removal while not iterating
    // 4. Fast insertion to the back of the structure
    std::mutex mutexM;
    std::map<int, CallBackTimer> timersM;
};