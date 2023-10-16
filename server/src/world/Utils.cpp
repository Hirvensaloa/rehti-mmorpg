#include <iostream>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>

#include "Utils.hpp"

bool isValidCell(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> coords, Direction dir)
{
  if (coords.first >= map.size() || coords.second >= map[0].size() || coords.first < 0 || coords.second < 0)
  {
    return false;
  }

  const unsigned &cell = map[coords.first][coords.second];

  switch (dir)
  {
  case Direction::NORTH:
    return cell == 0b0001;
  case Direction::EAST:
    return cell == 0b0010;
  case Direction::SOUTH:
    return cell == 0b0100;
  case Direction::WEST:
    return cell == 0b1000;
  case Direction::NORTH_EAST:
    return cell == 0b0011;
  case Direction::SOUTH_EAST:
    return cell == 0b0110;
  case Direction::SOUTH_WEST:
    return cell == 0b1100;
  case Direction::NORTH_WEST:
    return cell == 0b1001;
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

std::vector<std::pair<unsigned, unsigned>>
astar(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end)
{
  std::vector<std::pair<unsigned, unsigned>> path;
  unsigned mapHeight = map.size();
  unsigned mapWidth = std::max(map[start.first].size(), map[end.first].size()); // Map might not be a square, so we need to get the maximum width

  // Create the start and end nodes
  Node startNode = {start, 0, euclideanDistance(start, end)};
  Node endNode = {end, 0, 0};

  // Create the open priority queue and add the start node
  std::priority_queue<Node *, std::vector<Node *>, nodeCompare> open;
  open.push(&startNode);

  // Create the closed set, which contains all the nodes that have been visited
  std::vector<std::vector<bool>> closed(mapWidth, std::vector<bool>(mapHeight, false));

  while (!open.empty())
  {
    // Get the node with the lowest total cost
    Node *current = open.top();
    open.pop();

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
    closed[current->coords.first][current->coords.second] = true;

    // Check all the neighbours
    for (int i = 0; i < 8; i++)
    {
      // Get the coordinates of the neighbour
      std::pair<unsigned, unsigned> neighbourCoords = current->coords;
      switch (i)
      {
      case 0:
        neighbourCoords.first++;
        break;
      case 1:
        neighbourCoords.first++;
        neighbourCoords.second++;
        break;
      case 2:
        neighbourCoords.second++;
        break;
      case 3:
        neighbourCoords.first--;
        neighbourCoords.second++;
        break;
      case 4:
        neighbourCoords.first--;
        break;
      case 5:
        neighbourCoords.first--;
        neighbourCoords.second--;
        break;
      case 6:
        neighbourCoords.second--;
        break;
      case 7:
        neighbourCoords.first++;
        neighbourCoords.second--;
        break;
      }

      // Check if the neighbour is valid
      if (!isValidCell(map, neighbourCoords, static_cast<Direction>(i)))
      {
        continue;
      }

      // Check if the neighbour is in the closed set
      if (closed[neighbourCoords.first][neighbourCoords.second])
      {
        continue;
      }

      // Calculate the cost from start to the neighbour
      unsigned costFromStart = current->costFromStart + 1;

      // Check if the neighbour is in the open set
      Node *neighbour = nullptr;
      for (Node *node : open)
      {
        if (node->coords == neighbourCoords)
        {
          neighbour = node;
          break;
        }
      }
    }
  }
}

std::vector<std::pair<unsigned, unsigned>> findPath(const std::vector<std::vector<unsigned>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end)
{
  // Check that the start and end are within the map
  if (start.first >= map.size() || start.second >= map[start.first].size() || end.first >= map.size() || end.second >= map[end.first].size())
  {
    std::cerr << "Start or end is outside of the map!" << std::endl;
    return std::vector<std::pair<unsigned, unsigned>>();
  }

  // Check if the start and end are accessible
  const unsigned &startCell = map[start.first][start.second];
  const unsigned &endCell = map[end.first][end.second];

  if (startCell == 0 || endCell == 0)
  {
    std::cerr << "Start or end cell is not accessible! " << startCell << " " << endCell << std::endl;
    return std::vector<std::pair<unsigned, unsigned>>();
  }
}