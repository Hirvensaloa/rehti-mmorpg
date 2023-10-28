#pragma once

#include "Action.hpp"

class ObjectInteractAction : public Action
{
public:
  ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::string targetObjectIdM, Entity *pEntity);

  ~ObjectInteractAction() = default;

  const std::string getTargetId();

  void act();

private:
  const std::string targetObjectIdM;

  std::chrono::milliseconds actionTimeM = std::chrono::milliseconds(200);

  const ActionType actionTypeM = ActionType::ObjectInteract;
};