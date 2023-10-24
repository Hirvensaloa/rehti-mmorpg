#include "Object.hpp"

Object::Object(int id, std::string type, std::string name)
    : idM(id), typeM(type), nameM(name), instanceIdM(nextInstanceId++)
{
}

const int Object::getId()
{
  return idM;
}

const int Object::getInstanceId()
{
  return instanceIdM;
}

const std::string Object::getType()
{
  return typeM;
}

const std::string Object::getName()
{
  return nameM;
}
