#pragma once

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
  Node *parent = nullptr;

  unsigned totalCost()
  {
    return costFromStart + estimatedCostToGoal;
  };
};

// Check if cell is valid and accessible from given direction
bool isValidCell(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> cell, Direction dir);

// Calculate the euclidean distance between two points
unsigned euclideanDistance(std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);

/*
 * A-star algorithm made to work with the access map format. Allows diagonal movement and has a heuristic of euclidean distance.
 *
 * Returns a vector containing the path from start to finish.
 */
std::vector<std::pair<unsigned, unsigned>> astar(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);
