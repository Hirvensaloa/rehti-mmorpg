#include <iostream>

#include "Map.hpp"
#include "RehtiReader.hpp"
#include "Utils.hpp"

void Map::loadMap()
{
  accessMapM = fetchAccessMatrix();
  heightMapM = fetchHeightMatrix();
  std::cout << "Map loaded successfully!" << std::endl;
}

std::vector<std::pair<unsigned, unsigned>> Map::findPath(Coordinates start, Coordinates end)
{
  return astar(accessMapM, {start.x, start.y}, {end.x, end.y});
}

std::optional<int> Map::getHeight(int x, int y)
{
  if (x < 0 || y < 0 || y >= heightMapM.size() || x >= heightMapM[y].size())
  {
    return std::nullopt;
  }

  return heightMapM[y][x];
}