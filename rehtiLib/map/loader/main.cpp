#include <iostream>
#include <vector>
#include "src/utils.hpp"
#include "src/loader.hpp"

int main()
{
  const std::vector<std::vector<std::string>> areaMap = fetchAreaMap();
  const std::vector<std::vector<int>> heightMatrix = createHeightMap(areaMap);

  std::vector<std::vector<unsigned>> objectIdMap;
  const std::vector<std::vector<std::string>> objectBlockMap = createObjectBlockMap(areaMap, objectIdMap);

  // Generate access map for server
  const std::vector<std::vector<unsigned>> accessMap = generateAccessMap(heightMatrix, objectBlockMap);

  writeMatrixToFile(accessMap, GENERATED_ASSETS_PATH + "access_map.txt");

  std::cout << "Map assets generated successfully!" << std::endl;

  return 0;
}
