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
bool isValidCell(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> cell, Direction dir);

// Calculate the euclidean distance between two points
unsigned euclideanDistance(std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);

std::vector<std::pair<unsigned, unsigned>> astar(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);

/*
 * Finds a path from start to end using A* algorithm. Returns a vector containing the path from start to finish.
 * If no path is found, returns an empty vector.
 */
std::vector<std::pair<unsigned, unsigned>> findPath(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end);
