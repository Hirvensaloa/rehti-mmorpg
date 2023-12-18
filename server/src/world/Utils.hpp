#pragma once

#include "Coordinates.hpp"
#include "RehtiReader.hpp"

#include <memory>
#include <vector>

enum Direction
{
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST
};

struct Node
{
    std::pair<int, int> coords;
    float costFromStart;       ///< Number of steps from the start
    float estimatedCostToGoal; ///< Estimated number of steps to the goal, in our case the euclidean distance
    std::shared_ptr<Node> parent = nullptr;
    Direction dirFromParent; ///< Direction from the parent to this node

    float totalCost()
    {
        return costFromStart + estimatedCostToGoal;
    };
};

/**
 * @brief Check if cell is valid and accessible from given direction
 *
 * @param map The access map of the map.
 * @param cell The cell to check.
 * @param dir The direction from which the cell is accessed.
 * @return true if the cell is valid, false otherwise.
 */
bool isValidCell(const std::vector<std::vector<uint8_t>>& map, std::pair<int, int> cell, Direction dir);

/**
 * @brief Calculate the euclidean distance between two points
 *
 * @param start pair (x, y)
 * @param end pair (x, y)
 * @return Euclidean distance in tiles
 */
unsigned int euclideanDistance(std::pair<int, int> start, std::pair<int, int> end);

/**
 * @brief A-star algorithm made to work with the access map format. Allows diagonal movement and has a heuristic of euclidean distance.
 *
 * @param map
 * @param start
 * @param end
 * @return Returns a vector containing the path from start to finish. Start is not included in the path, but end is.
 */
std::vector<std::pair<int, int>> astar(const std::vector<std::vector<uint8_t>>& map, std::pair<int, int> start, std::pair<int, int> end);
