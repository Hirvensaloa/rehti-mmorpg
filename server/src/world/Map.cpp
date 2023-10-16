#include <iostream>

#include "Map.hpp"
#include "reader.hpp"
#include "Utils.hpp"

const std::string accessMapPath = "assets/generated/access_map.txt";

Map::Map()
{
  loadAccessMap(accessMapM, accessMapPath);
  std::cout << "Map loaded successfully!" << std::endl;
}

std::vector<std::pair<unsigned, unsigned>> Map::findPath(Coordinates start, Coordinates end)
{
  // Check that the start and end are within the map
  if (start.x >= accessMapM.size() || start.y >= accessMapM[start.x].size() || end.x >= accessMapM.size() || end.y >= accessMapM[end.x].size())
  {
    std::cerr << "Start or end is outside of the map!" << std::endl;
    return std::vector<std::pair<unsigned, unsigned>>();
  }

  // Check if the start and end are accessible
  const unsigned &startCell = accessMapM[start.x][start.y];
  const unsigned &endCell = accessMapM[end.x][end.y];

  if (startCell == 0 || endCell == 0)
  {
    std::cerr << "Start or end cell is not accessible! " << startCell << " " << endCell << std::endl;
    return std::vector<std::pair<unsigned, unsigned>>();
  }

  return astar(accessMapM, {start.x, start.y}, {end.x, end.y});
}
