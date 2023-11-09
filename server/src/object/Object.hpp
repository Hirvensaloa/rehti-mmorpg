#pragma once

#include <string>

#include "../entity/Entity.hpp"
#include "RehtiReader.hpp"

class Object
{
public:
  Object(int id, std::string instanceId, std::string name, Coordinates coordinates, unsigned int rotation, reader::ObjectType objectType);

  Object(const GeneralObjectStruct &object, const ObjectLocation &location, reader::ObjectType objectType);

  virtual ~Object() = default;

  const int &getId();
  const std::string &getInstanceId();
  const std::string &getName();
  const Coordinates &getLocation();
  const unsigned int &getRotation();
  const reader::ObjectType &getObjectType();

  virtual void interact(Entity &entity) { std::cout << "Cannot interact with this object" << std::endl; };

private:
  int idM;                 // Unique id for object type (For example, tree, rock, etc)
  std::string instanceIdM; // Unique id for object instance (For example, tree 1, tree at 2 etc)
  std::string nameM;
  Coordinates coordinatesM;
  unsigned int rotationM;
  reader::ObjectType objectTypeM;
};