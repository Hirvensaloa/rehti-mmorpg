#include "TileUtils.hpp"

TileHeight calculateTileHeights(const std::vector<std::vector<int>> &heightMap, const int row, const int col)
{
  int bottomNeighHeight = row + 1 < heightMap.size() && col < heightMap[row + 1].size() ? heightMap[row + 1][col] : heightMap[row][col];
  int rightNeighHeight = col + 1 < heightMap[row].size() ? heightMap[row][col + 1] : heightMap[row][col];
  int leftNeighHeight = col - 1 >= 0 ? heightMap[row][col - 1] : heightMap[row][col];
  int topNeighHeight = row - 1 >= 0 && col < heightMap[row - 1].size() ? heightMap[row - 1][col] : heightMap[row][col];
  int bottomRightNeighHeight = row + 1 < heightMap.size() && col + 1 < heightMap[row + 1].size() ? heightMap[row + 1][col + 1] : heightMap[row][col];
  int bottomLeftNeighHeight = row + 1 < heightMap.size() && col - 1 >= 0 && col - 1 < heightMap[row + 1].size() ? heightMap[row + 1][col - 1] : heightMap[row][col];
  int topRightNeighHeight = row - 1 >= 0 && col + 1 < heightMap[row - 1].size() ? heightMap[row - 1][col + 1] : heightMap[row][col];
  int topLeftNeighHeight = row - 1 >= 0 && col - 1 >= 0 ? heightMap[row - 1][col - 1] : heightMap[row][col];

  float bottomRightHeight = (heightMap[row][col] + bottomNeighHeight + rightNeighHeight + bottomRightNeighHeight) / 4.0;
  float bottomLeftHeight = (heightMap[row][col] + bottomNeighHeight + leftNeighHeight + bottomLeftNeighHeight) / 4.0;
  float topLeftHeight = (heightMap[row][col] + topNeighHeight + leftNeighHeight + topLeftNeighHeight) / 4.0;
  float topRightHeight = (heightMap[row][col] + topNeighHeight + rightNeighHeight + topRightNeighHeight) / 4.0;
  float bottomHeight = (heightMap[row][col] + bottomNeighHeight) / 2.0;
  float leftHeight = (heightMap[row][col] + leftNeighHeight) / 2.0;
  float topHeight = (heightMap[row][col] + topNeighHeight) / 2.0;
  float rightHeight = (heightMap[row][col] + rightNeighHeight) / 2.0;

  return {bottomRightHeight, bottomLeftHeight, topLeftHeight, topRightHeight, bottomHeight, leftHeight, topHeight, rightHeight};
};

std::pair<float, float> findMinMaxHeight(const TileHeight &tileHeight)
{
  float min = tileHeight.bottomRight;
  float max = tileHeight.bottomRight;
  std::vector f = {tileHeight.bottomLeft, tileHeight.topLeft, tileHeight.topRight, tileHeight.bottom, tileHeight.left, tileHeight.top, tileHeight.right};
  for (int i = 0; i < f.size(); i++)
  {
    if (f[i] < min)
    {
      min = f[i];
    }
    if (f[i] > max)
    {
      max = f[i];
    }
  }
  return {min, max};
}