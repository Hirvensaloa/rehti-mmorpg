#pragma once

#include <string>

#include "../entity/PlayerCharacter.hpp"

class Object
{
public:
  Object(int id, std::string instanceId, std::string name, Coordinates coordinates, unsigned int rotation);

  virtual ~Object();

  const int getId();
  const std::string getInstanceId();
  const std::string getName();
  const Coordinates getLocation();
  const unsigned int getRotation();

  virtual void interact(PlayerCharacter *player) { std::cout << "Cannot interact with this object" << std::endl; };

private:
  int idM;                 // Unique id for object type (For example, tree, rock, etc)
  std::string instanceIdM; // Unique id for object instance (For example, tree 1, tree at 2 etc)
  std::string nameM;
  Coordinates coordinatesM;
  unsigned int rotationM;
};