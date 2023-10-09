#pragma once

#include <string>

#include "Npc.hpp"

class Goblin : public Npc
{

public:
  Goblin(std::string name, unsigned int id = 0, Coordinates location = Coordinates());

  ~Goblin() = default;
};