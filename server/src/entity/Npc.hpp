#pragma once

#include <string>

#include "Entity.hpp"

class Npc : public Entity
{

public:
  Npc(std::string name, unsigned int id = 0, Coordinates location = Coordinates());

  ~Npc() = default;

  std::string getName();

  void update();

private:
  std::string nameM;
};