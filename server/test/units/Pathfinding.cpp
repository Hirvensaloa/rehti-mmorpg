#include <gtest/gtest.h>

#include "../../src/world/Utils.hpp"

TEST(AStarTest, SimpleCase)
{
  std::vector<std::vector<uint8_t>> map = {
      {0b0001, 0b0001, 0b0001},
      {0b0000, 0b0001, 0b0001},
      {0b0001, 0b0000, 0b0001}};
  std::pair<unsigned, unsigned> start = {0, 0};
  std::pair<unsigned, unsigned> end = {2, 2};

  std::vector<std::pair<unsigned, unsigned>> path = astar(map, start, end);

  ASSERT_EQ(path.size(), 5);   // Check if the path has the correct length
  ASSERT_EQ(path[0], start);   // Check if the start point is the first in the path
  ASSERT_EQ(path.back(), end); // Check if the end point is the last in the path
}

TEST(AStarTest, NoPath)
{
  std::vector<std::vector<uint8_t>> map = {
      {0b0001, 0b0000, 0b0001},
      {0b0000, 0b0000, 0b0001},
      {0b0001, 0b0000, 0b0001}};
  std::pair<unsigned, unsigned> start = {0, 0};
  std::pair<unsigned, unsigned> end = {2, 2};

  std::vector<std::pair<unsigned, unsigned>> path = astar(map, start, end);

  ASSERT_TRUE(path.empty());
}

TEST(AStarTest, InvalidStartEnd)
{
  std::vector<std::vector<uint8_t>> map = {
      {0b0001, 0b0001, 0b0001},
      {0b0000, 0b0001, 0b0001},
      {0b0001, 0b0000, 0b0001}};
  std::pair<unsigned, unsigned> start = {0, 0};
  std::pair<unsigned, unsigned> end = {3, 3}; // End point outside the map

  std::vector<std::pair<unsigned, unsigned>> path = astar(map, start, end);

  ASSERT_TRUE(path.empty());
}