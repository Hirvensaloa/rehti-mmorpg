#include "Object.hpp"

Object::Object(int id, std::string instanceId, std::string name, Coordinates location, unsigned int rotation, ObjectType objectType)
    : idM(id), instanceIdM(instanceId), nameM(name), coordinatesM(location), rotationM(rotation), objectTypeM(objectType)
{
}

Object::Object(const GeneralObjectStruct &object, const ObjectLocation &location, ObjectType objectType)
    : idM(object.id), instanceIdM(location.instanceId), nameM(object.name), coordinatesM({location.x, location.y, location.z}), rotationM(location.rotation), objectTypeM(objectType)
{
}

const int &Object::getId()
{
  return idM;
}

const std::string &Object::getInstanceId()
{
  return instanceIdM;
}

const std::string &Object::getName()
{
  return nameM;
}

const Coordinates &Object::getLocation()
{
  return coordinatesM;
}

const unsigned int &Object::getRotation()
{
  return rotationM;
}

const ObjectType &Object::getObjectType()
{
  return objectTypeM;
}