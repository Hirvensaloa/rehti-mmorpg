#pragma once

#include "../object/Object.hpp"
#include "../world/Coordinates.hpp"
#include "Action.hpp"
#include "MoveAction.hpp"

class ObjectInteractAction : public Action
{
public:
  ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::shared_ptr<Object> pTarget, std::shared_ptr<Entity> pEntity);

  ~ObjectInteractAction() = default;

  void act();

private:
  std::optional<Coordinates> findNextMove();

  std::shared_ptr<Object> pTargetM;

  std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(200);
  std::chrono::milliseconds moveTimeM = MoveAction::getMoveTime();

  const ActionType actionTypeM = ActionType::ObjectInteract;
};