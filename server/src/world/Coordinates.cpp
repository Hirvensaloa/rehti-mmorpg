#include <cmath>

#include "Coordinates.hpp"
#include "Map.hpp"

Coordinates::Coordinates(int x0, int y0) : x(x0), y(y0)
{
  std::optional<int> height = Map::getHeight(x, y);

  if (!height.has_value())
  {
    throw std::runtime_error("Coordinates out of bounds: x: " + std::to_string(x) + " y: " + std::to_string(y));
  }

  z = height.value();
}

bool Coordinates::operator==(Coordinates &other)
{
  return (x == other.x && y == other.y && z == other.z);
}

unsigned int Coordinates::distance(const Coordinates &other)
{
  return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
}