#include <string>

class ResourceObject : public Object
{
public:
  ResourceObject(int id, std::string type, std::string name, int yieldableItemId, int yieldableItemAmount);

  // Returns the id of the item that was received from opening the loot object. If no item was received, returns nullopt
  std::optional<int> mine();

private:
  int yieldableItemId;
  int yieldableItemAmount;
};