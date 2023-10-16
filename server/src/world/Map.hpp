#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <iostream>

#include "Coordinates.hpp"

class Map
{
public:
  Map();

  ~Map() = default;

  std::optional<std::vector<std::pair<uint8_t, uint8_t>>> findPath(Coordinates start, Coordinates end);

private:
  std::vector<std::vector<uint8_t>> accessMapM;
};
