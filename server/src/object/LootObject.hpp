#include <vector>
#include <optional>

#include "Object.hpp"

struct YieldableItem
{
  int id;
  int chance;
};

class LootObject : public Object
{
public:
  LootObject(int id, std::string type, std::string name, std::vector<YieldableItem> yieldableItemList);

  // Returns the id of the item that was received from opening the loot object. If no item was received, returns nullopt
  std::optional<int> open();

private:
  std::vector<YieldableItem> yieldableItemList;
};
