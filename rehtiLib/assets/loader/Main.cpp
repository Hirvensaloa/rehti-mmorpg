#include <iostream>
#include <vector>
#include <filesystem>

#include "src/Utils.hpp"
#include "src/MapLoader.hpp"
#include "src/MapObjLoader.hpp"

int main()
{
  // Create generated folder if it does not exist
  if (!std::filesystem::exists(Config.GENERATED_ASSETS_PATH))
  {
    std::filesystem::create_directory(Config.GENERATED_ASSETS_PATH);
  }

  // Fetch game assets
  std::map<int, GameSkill> gameSkills = fetchSkills();
  GameItems gameItems = fetchItems();
  GameObjects gameObjects = fetchObjects(gameItems, gameSkills);

  // Generate map assets
  const std::vector<std::vector<std::string>> areaMap = fetchAreaMap();
  const std::vector<std::vector<int>> heightMatrix = createHeightMap(areaMap);
  const std::vector<std::vector<std::string>> objectBlockMap = createObjectBlockMap(areaMap, gameObjects, heightMatrix);

  // Generate access map and height map for server
  std::cout << "here" << std::endl;
  const std::vector<std::vector<unsigned>> accessMap = generateAccessMap(heightMatrix, objectBlockMap);
  std::cout << "here 1" << std::endl;
  writeMatrixToFile(accessMap, Config.GENERATED_ACCESS_MAP_PATH);
  std::cout << "here 2" << std::endl;
  writeMatrixToFile(heightMatrix, Config.GENERATED_HEIGHT_MAP_PATH);
  std::cout << "here 3" << std::endl;
  writeMatrixToFile(areaMap, Config.GENERATED_AREA_MAP_PATH);

  // Generate obj map for client
  generateMapObj(heightMatrix);

  std::cout << "Map assets generated successfully!" << std::endl;

  std::cout << "----------------------------------------" << std::endl;
  std::cout << "Assets loaded and generated successfully!" << std::endl;

  return 0;
}
