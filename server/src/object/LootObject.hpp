#pragma once

#include <vector>
#include <optional>

#include "Object.hpp"

class LootObject : public Object
{
public:
  LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, ObjectType type);

  LootObject(const LootObjectStruct &object, const ObjectLocation &location, ObjectType objectType);

  void interact(Entity &entity);

private:
  std::vector<YieldableItem> yieldableItemListM;
};
