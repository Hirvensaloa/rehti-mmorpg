#pragma once

#include <vector>

/**
 * @brief Contains the height of each tile's corner and side.
 */
struct TileHeight
{
  float bottomRight, bottomLeft, topLeft, topRight, bottom, left, top, right;
};

/**
 * @brief Calculate the height of the tile's corners and sides, based on it's neighbours heights.
 *
 * @param heightMap contains height of each tile
 * @param row index of the tile
 * @param col index of the tile
 * @return TileHeight
 */
TileHeight calculateTileHeights(const std::vector<std::vector<int>> &heightMap, const int row, const int col);

/**
 * @brief Find minimum and maximum height of the tile.
 * @param tileHeight TileHeight
 * @return A pair, {minimum height,maximum height}
 */
std::pair<float, float> findMinMaxHeight(const TileHeight &tileHeight);