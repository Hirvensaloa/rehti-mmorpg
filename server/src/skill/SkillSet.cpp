#include <cmath>

#include "../utils/AssetManager.hpp"
#include "SkillSet.hpp"

SkillSet::SkillSet()
{
    const auto skills = AssetManager::getSkills();

    for (const auto& skill : skills)
    {
        int xp = 0;
        const std::string name = skill.second.name;
        skillsM.insert(std::pair<int, SkillInfo>(skill.first, SkillInfo{name, xp}));
    };
}

const std::map<int, SkillInfo>& SkillSet::getSkills()
{
    return skillsM;
};

std::optional<int> SkillSet::getSkillLevel(int id)
{
    if (!skillsM.contains(id))
    {
        return std::nullopt;
    }
    return static_cast<int>(std::floor(0.5 + std::sqrt(0.25 + 2 * getSkillXp(id).value() / 25)));
}

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