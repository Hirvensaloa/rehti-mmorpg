#include "LootObject.hpp"

LootObject::LootObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList)
    : Object(id, instanceId, name, coords, rotation), yieldableItemListM(yieldableItemList)
{
}

void LootObject::interact(PlayerCharacter *player)
{
  int rand = std::rand() % 100;
  int cumulativeChance = 0;

  for (YieldableItem item : yieldableItemListM)
  {
    if (cumulativeChance < rand && rand < cumulativeChance + item.yieldPercentage)
    {
      return;
    }
    cumulativeChance += item.yieldPercentage;
  }

  return;
}
