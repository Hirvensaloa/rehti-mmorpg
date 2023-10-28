#include "SkillSet.hpp"

SkillSet::SkillSet(AssetManager &assetManager)
{
  const auto skills = assetManager.getSkills();

  for (const auto &skill : skills)
  {
    int xp = 0; // TODO: Add XP from the database
    const std::string name = skill.second.name;
    skillsM.insert(std::pair<int, SkillInfo>(skill.first, SkillInfo{name, xp}));
  };
}

const std::map<int, SkillInfo> &SkillSet::getSkills()
{
  return skillsM;
};

std::optional<int> SkillSet::getSkillXp(int id)
{
  if (!skillsM.contains(id))
  {
    return std::nullopt;
  }

  return skillsM[id].xp;
}

bool SkillSet::addSkillXp(int id, unsigned int amount)
{
  if (!skillsM.contains(id))
  {
    return false;
  }

  skillsM[id].xp += amount;
  return true;
}