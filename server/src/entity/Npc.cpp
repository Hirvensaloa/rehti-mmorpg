#include "Npc.hpp"

#include <iostream>

Npc::Npc(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : Entity(pGameWorld, name, id, location){};

void Npc::update()
{
  // By default, NPCs just walk around
  if (currentActionM != nullptr && !currentActionM->isCompleted())
  {
    currentActionM->act();
  }
  else
  {
    const int walk = rand() % 100;
    if (walk)
    {
      setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Coordinates(rand() % 10, rand() % 10), this->shared_from_this()));
    }
  }
}

void Npc::respawn()
{
  getInventory().removeAllItems(); // TODO: drop them on the ground
  setAction(std::make_shared<RespawnAction>(std::chrono::system_clock::now(), respawnTimeM, this->shared_from_this()));
}
