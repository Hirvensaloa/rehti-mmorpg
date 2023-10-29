#pragma once

#include <map>
#include <memory>

#include "RehtiReader.hpp"
#include "../item/Item.hpp"

/*
 * Holds definitions to all the items, objects and skills in the game.
 * This class is used to fetch the definitions and create instances of the items, objects and skills.
 */
class AssetManager
{
public:
  static void loadAssets();

  static const std::vector<ObjectLocation> &getObjectLocations();

  static const std::map<int, GameSkill> &getSkills();

  static const GameObjects &getObjects();

  static const GameItems &getItems();

  // Nullptr if item id is not found
  static std::shared_ptr<Item> createItemInstance(int id);

private:
  inline static GameItems itemsM = {};
  inline static std::map<int, GameSkill> skillsM = {};
  inline static GameObjects objectsM = {};                         // Tells what possible objects exists and what attributes they have (For example tree)
  inline static std::vector<ObjectLocation> objectLocationsM = {}; // Tells where objects instances are located on the map (For example, tree at 1,1, tree at 2,2, tree at 4,5 etc)
};