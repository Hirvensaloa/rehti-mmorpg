#include "Object.hpp"

Object::Object(int id, std::string instanceId, std::string name, Coordinates location, unsigned int rotation)
    : idM(id), instanceIdM(instanceId), nameM(name), coordinatesM(location), rotationM(rotation)
{
}

Object::~Object() {}

const int Object::getId()
{
  return idM;
}

const std::string Object::getInstanceId()
{
  return instanceIdM;
}

const std::string Object::getName()
{
  return nameM;
}

const Coordinates Object::getLocation()
{
  return coordinatesM;
}

const unsigned int Object::getRotation()
{
  return rotationM;
}