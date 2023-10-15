#include "Map.hpp"
#include "reader.hpp"
#include <iostream>

const std::string accessMapPath = "assets/generated/access_map.txt";

Map::Map()
{
  loadAccessMap(accessMapM, accessMapPath);
  std::cout << "Map loaded successfully!" << std::endl;
}

std::optional<std::vector<uint8_t>> Map::findPath(Coordinates start, Coordinates end)
{
  // TODO
  return std::nullopt;
}
