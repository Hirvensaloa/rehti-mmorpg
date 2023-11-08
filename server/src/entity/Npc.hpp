#pragma once

#include <string>

#include "Entity.hpp"

class Npc : public Entity
{

public:
  Npc(GameWorld *pGameWorld, std::string name, unsigned int id = 0, Coordinates location = Coordinates());

  ~Npc() = default;

  void update();

  void respawn();
};