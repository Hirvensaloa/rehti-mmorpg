#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

#include "Coordinates.hpp"

class Map
{
public:
  Map();

  ~Map() = default;

  // Finds a path from start to end, returns empty vector if not found
  std::vector<std::pair<unsigned, unsigned>> findPath(Coordinates start, Coordinates end);

private:
  std::vector<std::vector<uint8_t>> accessMapM;
};
