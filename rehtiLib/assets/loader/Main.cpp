#include <iostream>
#include <vector>

#include "src/MapLoader.hpp"
#include "src/MapObjLoader.hpp"
#include "src/Utils.hpp"

int main()
{
  // Fetch game assets
  std::map<int, GameSkill> gameSkills = fetchSkills();
  GameItems gameItems = fetchItems();
  GameObjects gameObjects = fetchObjects(gameItems, gameSkills);

  // Generate map assets
  const std::vector<std::vector<std::string>> areaMap = fetchAreaMap();
  const std::vector<std::vector<int>> heightMatrix = createHeightMap(areaMap);
  const std::vector<std::vector<std::string>> objectBlockMap = createObjectBlockMap(areaMap, gameObjects, heightMatrix);

  // Generate access map for server
  const std::vector<std::vector<unsigned>> accessMap = generateAccessMap(heightMatrix, objectBlockMap);
  writeMatrixToFile(accessMap, GENERATED_ASSETS_PATH + "access_map.txt");

  // Generate obj map for client
  generateMapObj(heightMatrix);

  std::cout << "Map assets generated successfully!" << std::endl;

  std::cout << "----------------------------------------" << std::endl;
  std::cout << "Assets loaded and generated successfully!" << std::endl;

  return 0;
}
