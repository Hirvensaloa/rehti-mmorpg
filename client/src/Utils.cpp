#include "Utils.hpp"

AnimationConfig actionToAnimationConfig(CurrentAction action, Coordinate entityLocation)
{
    glm::vec3 targetVec = glm::vec3(action.targetCoordinate.x, Config.HEIGHT_MAP_SCALE * action.targetCoordinate.z, action.targetCoordinate.y);
    glm::vec3 entityVec = glm::vec3(entityLocation.x, Config.HEIGHT_MAP_SCALE * entityLocation.z, entityLocation.y);
    glm::vec3 targetDirection = targetVec - entityVec;
    // Keep the y axis between -0.2 and 0.2. This is to avoid the character from looking up or down too much
    targetDirection.y = std::min(std::max(targetDirection.y, -0.2f), 0.2f);
    switch (action.id)
    {
    case ActionType::Move:
        return AnimationConfig{targetDirection, AnimationType::WALK, static_cast<float>(action.durationMs), action.looping};
    case ActionType::ObjectInteract:
        return AnimationConfig{targetDirection, getAnimationType(AssetCache::getInstance().getObjectAssetDataById(action.targetId).characterInteractAnimation), static_cast<float>(action.durationMs), action.looping};
    case ActionType::Attack:
        return AnimationConfig{targetDirection, AnimationType::ATTACK, static_cast<float>(action.durationMs), action.looping};
    case ActionType::Respawn:
        return AnimationConfig{{0, 0, 0}, AnimationType::DEATH, static_cast<float>(action.durationMs), action.looping};
    default:
        return AnimationConfig{{0, 0, 0}, AnimationType::IDLE, 1000.f, true};
    }
}