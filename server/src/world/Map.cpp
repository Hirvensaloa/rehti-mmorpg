#include <iostream>

#include "Map.hpp"
#include "Utils.hpp"

const std::string accessMapPath = "assets/generated/access_map.txt";

Map::Map()
{
  loadAccessMap(accessMapM, accessMapPath);
  std::cout << "Map loaded successfully!" << std::endl;
}

std::vector<std::pair<unsigned, unsigned>> Map::findPath(Coordinates start, Coordinates end)
{
  return astar(accessMapM, {start.x, start.y}, {end.x, end.y});
}
