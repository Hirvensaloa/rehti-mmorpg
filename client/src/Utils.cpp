#include "Utils.hpp"

AnimationConfig actionToAnimationConfig(CurrentAction action)
{
    switch (action.id)
    {
    case ActionType::Move:
        return AnimationConfig{AnimationType::RUN, static_cast<float>(action.durationMs), action.looping};
    case ActionType::ObjectInteract:
        return AnimationConfig{getAnimationType(AssetCache::getInstance().getObjectAssetDataById(action.targetId).characterInteractAnimation), static_cast<float>(action.durationMs), action.looping};
    case ActionType::Attack:
        return AnimationConfig{AnimationType::ATTACK, static_cast<float>(action.durationMs), action.looping};
    default:
        return AnimationConfig{AnimationType::IDLE, static_cast<float>(action.durationMs), action.looping};
    }
}