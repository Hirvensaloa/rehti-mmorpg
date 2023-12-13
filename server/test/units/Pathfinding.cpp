#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/world/Utils.hpp"

TEST(AStarTest, SimpleCase)
{
  std::vector<std::vector<uint8_t>> map = {
      {0b0010, 0b1100, 0b0001},
      {0b0000, 0b0101, 0b0001},
      {0b0001, 0b0011, 0b1000}};
  std::pair<unsigned, unsigned> start = {0, 0};
  std::pair<unsigned, unsigned> end = {2, 2};

  std::vector<std::pair<unsigned, unsigned>> path = astar(map, start, end);

  ASSERT_THAT(path, testing::ElementsAreArray({{0, 1}, {1, 1}, {2, 1}, end})); // Check if the path is correc
}

TEST(AStartTest, ComplexCase)
{
  // Check a map that is not a rectangle
  std::vector<std::vector<uint8_t>> map = {
      {0b0010, 0b1100, 0b0001},
      {0b0000, 0b0101, 0b0001, 0b1000},
      {0b0001, 0b0101, 0b1000, 0b1010},
      {0b0001, 0b0011, 0b1001}};

  std::pair<unsigned, unsigned> start = {0, 0};
  std::pair<unsigned, unsigned> end = {2, 3};

  std::vector<std::pair<unsigned, unsigned>> path = astar(map, start, end);

  ASSERT_THAT(path, testing::ElementsAreArray({{0, 1}, {1, 1}, {2, 1}, {2, 2}, end})); // Check if the path is correct
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