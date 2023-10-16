#include <iostream>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>

#include "Utils.hpp"

bool isValidCell(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> coords, Direction dir)
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

std::vector<std::pair<unsigned, unsigned>> astar(const std::vector<std::vector<uint8_t>> &map, std::pair<unsigned, unsigned> start, std::pair<unsigned, unsigned> end)
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

  // Create the closed set, which contains all the nodes that have been visited. ALSO contains a pointer to the open set node, if the node is in the open set (This is because prio queue access is O(n))
  std::vector<std::vector<std::pair<bool, Node *>>> closed(mapWidth, std::vector<std::pair<bool, Node *>>(mapHeight, {false, nullptr}));

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
    closed[current->coords.first][current->coords.second].first = true;

    // Check all the neighbours
    for (int i = 0; i < 8; i++)
    {
      // Get the coordinates of the neighbour
      std::pair<unsigned, unsigned> neighbourCoords = current->coords;
      switch (i)
      {
      case NORTH:
        neighbourCoords.first++;
        break;
      case NORTH_EAST:
        neighbourCoords.first++;
        neighbourCoords.second++;
        break;
      case EAST:
        neighbourCoords.second++;
        break;
      case SOUTH_EAST:
        neighbourCoords.first--;
        neighbourCoords.second++;
        break;
      case SOUTH:
        neighbourCoords.first--;
        break;
      case SOUTH_WEST:
        neighbourCoords.first--;
        neighbourCoords.second--;
        break;
      case WEST:
        neighbourCoords.second--;
        break;
      case NORTH_WEST:
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
