#include <iostream>

#include "Map.hpp"
#include "Utils.hpp"

void Map::loadMap()
{
    accessMapM = fetchAccessMatrix();
    heightMapM = fetchHeightMatrix();
    std::cout << "Map loaded successfully!" << std::endl;
}

std::vector<std::pair<int, int>> Map::findPath(Coordinates start, Coordinates end)
{
    return astar(accessMapM, {start.x, start.y}, {end.x, end.y});
}

std::vector<std::pair<int, int>> Map::findPathToRange(Coordinates start, Coordinates end, int range)
{
    // First construct all the possible locations that are in range.
    std::vector<Coordinates> possibleLocations;
    for (int i = end.x - range; i <= end.x + range; i++)
    {
        for (int j = end.y - range; j <= end.y + range; j++)
        {
            if (i >= 0 && j >= 0 && i < accessMapM.size() && j < accessMapM[i].size() && accessMapM[i][j] != 0)
            {
                possibleLocations.push_back(Coordinates(i, j));
            }
        }
    }

    // Order the locations by distance to the starting location.
    std::sort(possibleLocations.begin(), possibleLocations.end(), [start](Coordinates a, Coordinates b)
              { return start.distance(a) < start.distance(b); });

    // Find a path to the closest location.
    for (auto location : possibleLocations)
    {
        auto path = findPath(start, location);
        if (path.size() > 0)
        {
            return path;
        }
    }

    return {};
}

std::optional<int> Map::getHeight(int x, int y)
{
    if (x < 0 || y < 0 || y >= heightMapM.size() || x >= heightMapM[y].size())
    {
        return std::nullopt;
    }

    return heightMapM[y][x];
}

const std::vector<std::vector<uint8_t>>& Map::getAccessMap()
{
    return accessMapM;
}

Coordinates Map::getRandomCoordinates(const SpawnCoordinateBounds& coordinateBounds)
{
    // Used to check if coordinates are valid.
    const std::vector<std::vector<uint8_t>>& accessMatrix = getAccessMap();
    int x = rand() % (coordinateBounds.xMax - coordinateBounds.xMin + 1) + coordinateBounds.xMin;
    int y = rand() % (coordinateBounds.yMax - coordinateBounds.yMin + 1) + coordinateBounds.yMin;
    int randomCounter = x * y;

    bool hasNoAccess = true;
    // Loop through the accessMatrix randomCounter times and find a random coordinate that is accessible. Decrease the randomCounter by 1 each time a coordinate is accessible.
    while (randomCounter > 0)
    {
        for (int i = coordinateBounds.yMin; i < coordinateBounds.yMax; i++)
        {
            for (int j = coordinateBounds.xMin; j < coordinateBounds.xMax; j++)
            {
                if (accessMatrix[i].size() > j && accessMatrix[i][j] != 0) // We need to check if the accessMatrix[i] has j as an index because the accessMatrix is not a square matrix.
                {
                    hasNoAccess = false;
                    randomCounter--;
                    if (randomCounter == 0)
                    {
                        x = j;
                        y = i;
                        break;
                    }
                }
            }
        }

        if (hasNoAccess)
        {
            throw std::runtime_error("No accessible coordinates found. xMin: " + std::to_string(coordinateBounds.xMin) + " xMax: " + std::to_string(coordinateBounds.xMax) + " yMin: " + std::to_string(coordinateBounds.yMin) + " yMax: " + std::to_string(coordinateBounds.yMax));
        }
    }

    return Coordinates(x, y);
}

Coordinates Map::getRandomNeighbour(Coordinates coordinates, const SpawnCoordinateBounds& coordinateBounds)
{
    const std::vector<std::vector<uint8_t>>& accessMatrix = Map::getAccessMap();
    std::vector<Coordinates> neighbours;
    for (int i = coordinates.y - 1; i <= coordinates.y + 1; i++)
    {
        for (int j = coordinates.x - 1; j <= coordinates.x + 1; j++)
        {
            if (i >= 0 && j >= 0 && i < accessMatrix.size() && j < accessMatrix[i].size() && accessMatrix[i][j] != 0 && coordinateBounds.xMin <= j && j <= coordinateBounds.xMax && coordinateBounds.yMin <= i && i <= coordinateBounds.yMax)
            {
                neighbours.push_back(Coordinates(j, i));
            }
        }
    }

    if (neighbours.size() == 0)
    {
        return coordinates;
    }

    return neighbours[rand() % neighbours.size()];
}