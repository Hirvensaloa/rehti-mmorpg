#include "Utils.hpp"

AnimationConfig actionToAnimationConfig(CurrentAction action)
{
    switch (action.id)
    {
    case ActionType::Move:
        return AnimationConfig{glm::vec3(1.f), AnimationType::WALK, static_cast<float>(action.durationMs), action.looping};
    case ActionType::ObjectInteract:
        return AnimationConfig{glm::vec3(1.f), getAnimationType(AssetCache::getInstance().getObjectAssetDataById(action.targetId).characterInteractAnimation), static_cast<float>(action.durationMs), action.looping};
    case ActionType::Attack:
        return AnimationConfig{glm::vec3(1.f), AnimationType::ATTACK, static_cast<float>(action.durationMs), action.looping};
    default:
        return AnimationConfig{glm::vec3(1.f), AnimationType::IDLE, 1000.f, true};
    }
}