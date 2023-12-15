#pragma once

#include <map>
#include <optional>
#include <string>

/**
 * @brief Each player has a set of skills. This struct contains the name of the skill and the current xp in the skill.
 */
struct SkillInfo
{
    const std::string name;
    int xp;
};

/**
 * @brief Contains all the skills that a character has
 */
class SkillSet
{
public:
    SkillSet();

    /**
     * @brief Returns map with skill id keys and SkillInfo values
     * @return const std::map<int, SkillInfo>&
     */
    const std::map<int, SkillInfo>& getSkills();

    /**
     * @brief Returns the current xp of the skill with the given id. Returns nullopt if the skill doesn't exist
     * @param id
     * @return std::optional<int>
     */
    std::optional<int> getSkillXp(int id);

    /**
     * @brief Returns the level of the skill calculated based on the current xp in the skill. Returns nullopt if the skill doesn't exist.
     * @param id
     * @return std::optional<int>
     */
    std::optional<int> getSkillLevel(int id);

    /**
     * @brief Adds the given amount of xp to the skill with the given id. Returns false if the addition failed, for example skill doesn't exist.
     * @param id
     * @param amount
     * @return bool
     */
    bool addSkillXp(int id, unsigned int amount);

private:
    std::map<int, SkillInfo> skillsM; ///< Key = skill id, Value = SkillInfo
};