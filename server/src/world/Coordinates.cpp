#include <cmath>
#include <string>
#include <tuple>

#include "Coordinates.hpp"
#include "Map.hpp"

Coordinates::Coordinates(int x0, int y0) : x(x0), y(y0)
{
    std::optional<int> height = Map::getHeight(x, y);

    if (height.has_value())
    {
        z = height.value();
    }
}

bool Coordinates::operator==(Coordinates& other)
{
    return (x == other.x && y == other.y && z == other.z);
}

bool operator<(const Coordinates& l, const Coordinates& r)
{
    return std::tie(l.x, l.y, l.z) < std::tie(r.x, r.y, r.z);
}

unsigned int Coordinates::distance(const Coordinates& other)
{
    return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
}