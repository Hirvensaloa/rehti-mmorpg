#pragma once

#include <map>
#include <string>
#include <optional>

#include "../utils/AssetManager.hpp"

struct SkillInfo
{
  const std::string name;
  int xp;
};

// Contains all the skills that a character has
class SkillSet
{
public:
  SkillSet(AssetManager &assetManager);

  const std::map<int, SkillInfo> &getSkills();

  // Returns the current xp of the skill with the given id. Returns nullopt if the skill doesn't exist.
  std::optional<int> getSkillXp(int id);

  // Adds the given amount of xp to the skill with the given id. Returns false if the addition failed, for example skill doesn't exist.
  bool addSkillXp(int id, unsigned int amount);

private:
  std::map<int, SkillInfo> skillsM; // Key = skill id, Value = SkillInfo
};