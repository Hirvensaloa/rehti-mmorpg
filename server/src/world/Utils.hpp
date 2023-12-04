#pragma once

#include "Coordinates.hpp"
#include "RehtiReader.hpp"

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
    std::pair<unsigned, unsigned> coords;
    unsigned costFromStart;       // Number of steps from the start
    unsigned estimatedCostToGoal; // Estimated number of steps to the goal, in our case the euclidean distance
    Node* parent = nullptr;

    unsigned totalCost()
    {
        return costFromStart + estimatedCostToGoal;
    };
};

// Check if cell is valid and accessible from given direction
bool isValidCell(const std::vector<std::vector<uint8_t>>& map, std::pair<unsigned, unsigned> cell, Direction dir);

// Calculate the euclidean distance between two points
unsigned euclideanDistance(std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);

/*
 * A-star algorithm made to work with the access map format. Allows diagonal movement and has a heuristic of euclidean distance.
 *
 * Returns a vector containing the path from start to finish. Start is not included in the path, but end is.
 */
std::vector<std::pair<unsigned, unsigned>> astar(const std::vector<std::vector<uint8_t>>& map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);

/**
 * @brief Gets a random coordinate within the given bounds.
 *
 * @param spawnCoordinateBounds
 * @param accessMatrix The access matrix of the map. Used to check if coordinates are valid.
 * @return Coordinates
 */
Coordinates getRandomCoordinates(const SpawnCoordinateBounds& spawnCoordinateBounds, const std::vector<std::vector<uint8_t>>& accessMatrix);
