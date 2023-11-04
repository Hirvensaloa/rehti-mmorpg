#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "rapidjson/document.h"
#include "RehtiUtils.hpp"
#include "SkillReader.hpp"
#include "../../Config.hpp"

std::map<int, GameSkill> fetchSkills()
{
  rapidjson::Document document = readJson(Config.SKILL_JSON_PATH);

  // Ensure the root is an object
  if (!document.IsObject())
  {
    throw std::runtime_error("Skills JSON file is not an object.");
  }

  if (!validMember(document, "skills", ValueType::ARRAY))
  {
    throw std::runtime_error("Skills JSON file does not contain a skills array.");
  }

  std::map<int, GameSkill> skillsMap;

  rapidjson::Value &skillsArray = document["skills"];

  for (rapidjson::SizeType i = 0; i < skillsArray.Size(); i++)
  {
    const rapidjson::Value &skill = skillsArray[i];

    if (!validMember(skill, "id", ValueType::INT) || !validMember(skill, "name", ValueType::STRING) || !validMember(skill, "description", ValueType::STRING))
    {
      throw std::runtime_error("JSON Validation failed for skill index: " + std::to_string(i));
    }

    int id = skill["id"].GetInt();
    std::string name = skill["name"].GetString();
    std::string description = skill["description"].GetString();
    GameSkill skillObject = {name, description};

    skillsMap[id] = skillObject;
  }

  // Display the skills stored in the map
  for (const auto &skill : skillsMap)
  {
    std::cout << "Skill ID: " << skill.first << ", Name: " << skill.second.name << std::endl;
  }

  return skillsMap;
}
