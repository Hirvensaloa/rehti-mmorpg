#include <iostream>

#include "ObjectInteractAction.hpp"

ObjectInteractAction::ObjectInteractAction(std::chrono::system_clock::time_point startTime, std::string targetObjectId, Entity *pEntity) : Action(startTime, pEntity), targetObjectIdM(targetObjectId) {}

Coordinates ObjectInteractAction::getTargetId()
{
  return targetObjectIdM;
}

void ObjectInteractAction::act()
{
  if (!completedM)
  {
    if (std::chrono::system_clock::now() > startTimeM + actionTimeM)
    {
      pEntityM->move(targetM);
      startTimeM = std::chrono::system_clock::now();
      if (pEntityM->getLocation() == targetM)
      {
        completedM = true;
      }
    }
  }
}