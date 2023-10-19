#include <iostream>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>

#include "Utils.hpp"

bool isValidCell(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> coords, Direction dir)
{
  if (coords.first >= map.size() || coords.second >= map[coords.first].size() || coords.first < 0 || coords.second < 0)
  {
    return false;
  }

  const uint8_t &cell = map[coords.first][coords.second];

  switch (dir)
  {
  case Direction::NORTH:
    return (cell & 0b0001) != 0;
  case Direction::EAST:
    return (cell & 0b0010) != 0;
  case Direction::SOUTH:
    return (cell & 0b0100) != 0;
  case Direction::WEST:
    return (cell & 0b1000) != 0;
  case Direction::NORTH_EAST:
    return (cell & 0b0011) == 0b0011;
  case Direction::SOUTH_EAST:
    return (cell & 0b0110) == 0b0110;
  case Direction::SOUTH_WEST:
    return (cell & 0b1100) == 0b1100;
  case Direction::NORTH_WEST:
    return (cell & 0b1001) == 0b1001;
  default:
    return false;
  }
}

unsigned euclideanDistance(std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end)
{
  return std::sqrt(std::pow(end.first - start.first, 2) + std::pow(end.second - start.second, 2));
}

struct nodeCompare
{
  bool operator()(Node *a, Node *b)
  {
    return a->totalCost() > b->totalCost();
  }
};

std::vector<std::pair<unsigned, unsigned>> astar(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end)
{
  // Check that the start and end are within the map
  if (start.first >= map.size() || start.second >= map[start.first].size() || end.first >= map.size() || end.second >= map[end.first].size())
  {
    return std::vector<std::pair<unsigned, unsigned>>();
  }

  // Check if the start and end are accessible
  const unsigned &startCell = map[start.first][start.second];
  const unsigned &endCell = map[end.first][end.second];

  if (startCell == 0 || endCell == 0)
  {
    return std::vector<std::pair<unsigned, unsigned>>();
  }

  std::vector<std::pair<unsigned, unsigned>> path;
  unsigned mapHeight = map.size();
  unsigned mapWidth = std::max(map[start.first].size(), map[end.first].size()); // Map might not be a square, so we need to get the maximum width

  // Create the start and end nodes
  Node startNode = {start, 0, euclideanDistance(start, end)};
  Node endNode = {end, 0, 0};

  // Create the open priority queue and add the start node
  std::priority_queue<Node *, std::vector<Node *>, nodeCompare> open;
  open.push(&startNode);

  // Create the closed set, which contains all the nodes that have been visited. ALSO contains a pointer to the open set node, if the node is in the open set (This is because prio queue access is O(n))
  std::vector<std::vector<std::pair<bool, Node *>>> closed(mapHeight, std::vector<std::pair<bool, Node *>>(mapWidth, {false, nullptr}));

  while (!open.empty())
  {
    // Get the node with the lowest total cost
    Node *current = open.top();
    open.pop();
    closed[current->coords.first][current->coords.second].second = nullptr;

    // Check if the current node is the end node
    if (current->coords == endNode.coords)
    {
      // Reconstruct the path
      Node *node = current;
      while (node->parent != nullptr)
      {
        path.push_back(node->coords);
        node = node->parent;
      }
      std::reverse(path.begin(), path.end());
      return path;
    }

    // Add the current node to the closed set
    closed[current->coords.first][current->coords.second].first = true;

    // Check all the neighbours
    for (int i = 0; i < 8; i++)
    {
      // Get the coordinates of the neighbour
      std::pair<unsigned, unsigned> neighbourCoords = current->coords;
      Direction checkDir; // The direction of the current cell from the neighbours perspective
      switch (i)
      {
      case NORTH:
        neighbourCoords.first--;
        checkDir = SOUTH;
        break;
      case NORTH_EAST:
        neighbourCoords.first--;
        neighbourCoords.second++;
        checkDir = SOUTH_WEST;
        break;
      case EAST:
        neighbourCoords.second++;
        checkDir = WEST;
        break;
      case SOUTH_EAST:
        neighbourCoords.first++;
        neighbourCoords.second++;
        checkDir = NORTH_WEST;
        break;
      case SOUTH:
        neighbourCoords.first++;
        checkDir = NORTH;
        break;
      case SOUTH_WEST:
        neighbourCoords.first++;
        neighbourCoords.second--;
        checkDir = NORTH_EAST;
        break;
      case WEST:
        neighbourCoords.second--;
        checkDir = EAST;
        break;
      case NORTH_WEST:
        neighbourCoords.first--;
        neighbourCoords.second--;
        checkDir = SOUTH_EAST;
        break;
      }

      // Check if the neighbour is valid and accessible from OUR direction
      if (!isValidCell(map, neighbourCoords, checkDir))
      {
        continue;
      }

      // Check if the neighbour is in the closed set
      if (closed[neighbourCoords.first][neighbourCoords.second].first)
      {
        continue;
      }

      // Calculate the cost from start to the neighbour
      unsigned costFromStart = current->costFromStart + 1;

      // Check if the neighbour is in the open set
      Node *neighbour = closed[neighbourCoords.first][neighbourCoords.second].second;

      if (neighbour == nullptr)
      {
        // Create the neighbour node
        neighbour = new Node{neighbourCoords, costFromStart, euclideanDistance(neighbourCoords, endNode.coords), current};

        // Add the neighbour to the open set
        open.push(neighbour);
        closed[neighbourCoords.first][neighbourCoords.second].second = neighbour;
      }
      // Check if the cost from start is lower than the previous cost from start
      else if (costFromStart < neighbour->costFromStart)
      {
        // Update the neighbour
        neighbour->parent = current;
        neighbour->costFromStart = costFromStart;
      }
    }
  }

  // No path found, return an empty vector
  return path;
}
