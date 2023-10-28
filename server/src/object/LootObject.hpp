#pragma once

#include <vector>
#include <optional>

#include "Object.hpp"

class LootObject : public Object
{
public:
  LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList);

  void interact(PlayerCharacter *player);

private:
  std::vector<YieldableItem> yieldableItemListM;
};
