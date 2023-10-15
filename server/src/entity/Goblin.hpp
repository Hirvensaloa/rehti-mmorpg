#pragma once

#include <string>

#include "Npc.hpp"

class Goblin : public Npc
{

public:
  Goblin(GameWorld *pGameWorld, std::string name, unsigned int id = 0, Coordinates location = Coordinates());

  ~Goblin() = default;
};