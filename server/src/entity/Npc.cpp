#include "Npc.hpp"

#include <iostream>

Npc::Npc(std::string name, unsigned int id, Coordinates location) : Entity(id, location), nameM(name){};

void Npc::update()
{
  // By default, NPCs just walk around
  if (currentActionM && !currentActionM->isCompleted())
  {
    currentActionM->act();
  }
  else
  {
    const int walk = rand() % 100;
    if (walk)
    {
      setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Coordinates(rand() % 10, rand() % 10), this));
    }
  }
};

std::string Npc::getName()
{
  return nameM;
};