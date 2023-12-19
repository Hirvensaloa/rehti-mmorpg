#include "Utils.hpp"

AnimationConfig actionToAnimationConfig(CurrentAction action, Coordinate entityLocation)
{
    glm::vec3 targetDirection = glm::vec3(action.targetCoordinate.x - entityLocation.x, action.targetCoordinate.y - entityLocation.y, action.targetCoordinate.z - entityLocation.z);

    switch (action.id)
    {
    case ActionType::Move:
        return AnimationConfig{targetDirection, AnimationType::WALK, static_cast<float>(action.durationMs), action.looping};
    case ActionType::ObjectInteract:
        return AnimationConfig{targetDirection, getAnimationType(AssetCache::getInstance().getObjectAssetDataById(action.targetId).characterInteractAnimation), static_cast<float>(action.durationMs), action.looping};
    case ActionType::Attack:
        return AnimationConfig{targetDirection, AnimationType::ATTACK, static_cast<float>(action.durationMs), action.looping};
    default:
        return AnimationConfig{{0, 0, 0}, AnimationType::IDLE, 1000.f, true};
    }
}