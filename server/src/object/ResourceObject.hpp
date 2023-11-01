#include <string>
#include <vector>
#include <optional>

#include "Object.hpp"
#include "../entity/Entity.hpp"

class ResourceObject : public Object
{
public:
  ResourceObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, int xpPerYield, int depleteChance, int relatedSkillId, int xpRequirement, ObjectType type);

  ResourceObject(const ResourceObjectStruct &object, const ObjectLocation &location, ObjectType type);

  void interact(Entity &entity);

private:
  std::vector<YieldableItem> yieldableItemListM;
  int xpPerYieldM;
  int depleteChanceM; // TODO: Deplete functionality
  int relatedSkillIdM;
  int xpRequirementM;
};