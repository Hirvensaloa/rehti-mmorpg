#pragma once

#include "Utils.hpp"
#include <map>
#include <string>

struct GameSkill
{
    std::string name;
    std::string description;
};

/**
 * @brief Fetches the skills from the skills.json file and returns them as a map
 *
 * @return std::map<int, GameSkill>
 */
std::map<int, GameSkill> fetchSkills();
