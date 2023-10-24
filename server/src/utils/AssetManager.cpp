#include "AssetManager.hpp"

AssetManager::AssetManager()
{
  itemsM = fetchItems();
  skillsM = fetchSkills();
  objectsM = fetchObjects(itemsM, skillsM);
  objectLocationsM = readObjectLocations();
}