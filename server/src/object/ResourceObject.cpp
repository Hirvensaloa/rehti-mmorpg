#include "ResourceObject.hpp"

ResourceObject::ResourceObject(int id, std::string instanceId, std::string name, Coordinates coords, unsigned int rotation, std::vector<YieldableItem> yieldableItemList, int xpPerYield, int depleteChance, int relatedSkillId, int xpRequirement) : Object(id, instanceId, name, coords, rotation)
{
  this->yieldableItemListM = yieldableItemList;
  this->xpPerYieldM = xpPerYield;
  this->depleteChanceM = depleteChance;
  this->relatedSkillIdM = relatedSkillId;
  this->xpRequirementM = xpRequirement;
}

void ResourceObject::interact(PlayerCharacter *player)
{
  int rand = std::rand() % 100;
  int cumulativeChance = 0;

  for (YieldableItem item : yieldableItemListM)
  {
    if (cumulativeChance < rand && rand < cumulativeChance + item.yieldPercentage)
    {
      player->getSkillSet().addSkillXp(relatedSkillIdM, xpPerYieldM);
      return;
    }
    cumulativeChance += item.yieldPercentage;
  }

  return;
}