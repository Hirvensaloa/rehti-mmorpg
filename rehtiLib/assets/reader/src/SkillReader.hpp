#pragma once

#include <string>
#include <map>
#include "Utils.hpp"

struct GameSkill
{
  std::string name;
  std::string description;
};

// Fetches the skills from the skills.json file and returns them as a map
std::map<int, GameSkill> fetchSkills();
