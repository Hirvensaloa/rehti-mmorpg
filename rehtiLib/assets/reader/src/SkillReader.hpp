#pragma once

#include <string>
#include <map>
#include "Utils.hpp"

// Path(s)
const std::string SKILL_JSON_PATH = ROOT_PATH + "assets/skills.json";

struct GameSkill
{
  std::string name;
  std::string description;
};

// Fetches the skills from the skills.json file and returns them as a map
std::map<int, GameSkill> fetchSkills();
