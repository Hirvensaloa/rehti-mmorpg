#include "LootObject.hpp"

LootObject::LootObject(int id, std::string type, std::string name, std::vector<YieldableItem> yieldableItemList)
    : Object(id, type, name), yieldableItemList(yieldableItemList)
{
}

std::optional<int> LootObject::open()
{
  int rand = std::rand() % 100;
  int cumulativeChance = 0;

  for (YieldableItem item : yieldableItemList)
  {
    if (cumulativeChance < rand && rand < cumulativeChance + item.chance)
    {
      return item.id;
    }
    cumulativeChance += item.chance;
  }

  return nullopt;
}
