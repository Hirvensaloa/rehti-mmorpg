#include <map>

#include "ItemReader.hpp"
#include "ObjectReader.hpp"
#include "SkillReader.hpp"

/*
 * Holds definitions to all the items, objects and skills in the game.
 * This class is used to fetch the definitions and create instances of the items, objects and skills.
 */
class AssetManager
{
public:
  const std::vector<ObjectLocation> getObjectLocations();

private:
  GameItems itemsM;
  std::map<int, GameSkill> skillsM;
  GameObjects objectsM;                         // Tells what possible objects exists and what attributes they have (For example tree)
  std::vector<ObjectLocation> objectLocationsM; // Tells where objects instances are located on the map (For example, tree at 1,1, tree at 2,2, tree at 4,5 etc)
};