#include "AssetManager.hpp"

AssetManager::AssetManager(){};

void AssetManager::loadAssets()
{
  std::cout << ">>>>>> Loading assets..."
            << std::endl;

  itemsM = fetchItems();
  skillsM = fetchSkills();
  objectsM = fetchObjects(itemsM, skillsM);
  objectLocationsM = readObjectLocations();

  std::cout << ">>>>>> Assets loaded successfully!\n"
            << std::endl;
};

const std::vector<ObjectLocation> &AssetManager::getObjectLocations()
{
  return objectLocationsM;
};

const std::map<int, GameSkill> &AssetManager::getSkills()
{
  return skillsM;
};

const GameObjects &AssetManager::getObjects()
{
  return objectsM;
};