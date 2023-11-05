#include <memory>
#include <vector>
#include <string>
#include <glm/vec3.hpp>

/**
 * @file Implements AABB generation.
 */

/**
 * @brief Axis Aligned Bounding Box data structure
 */
struct AABB
{
  glm::vec3 min;
  glm::vec3 max;
  std::unique_ptr<AABB> pLeft;
  std::unique_ptr<AABB> pRight;
  bool isLeaf() const;
  glm::vec3 getCenter() const;
};

/**
 * @brief Constructs a minimum vector from two vectors.
 *
 * @param a vector a
 * @param b vector b
 * @return minimum vector
 */
glm::vec3 minVector(const glm::vec3 &a, const glm::vec3 &b);

/**
 * @brief Constructs a maximum vector from two vectors.
 *
 * @param a vector a
 * @param b vector b
 * @return maximum vector
 */
glm::vec3 maxVector(const glm::vec3 &a, const glm::vec3 &b);

/**
 * @brief Helper struct to store all the data necessary for creating a map AABB tree.
 */
struct MapAABBData
{
  const std::vector<std::vector<int>> heightMatrix;       ///< Matrix cell contains the height of the tile for the given column (x) and row (y). This matrix contains all the areas e.g. the map.
  const std::vector<std::vector<std::string>> areaMatrix; ///< Matrix cell contains the name of the area for the given column (x) and row (y). This gives important information about how the areas are connected.
  const unsigned int areaSize;                            ///< The size of the area in tiles. Width and height of the area are the same.
  const float heightScale;                                ///< Percentage value of how much the height is scaled from the original unit.
  const float sideUnit;                                   ///< The size of the side of a tile in the original unit.
  const float sideScale;                                  ///< Percentage value of how much the side is scaled from the original unit.
};

/**
 * @brief Creates a list of AABBs trees for each area in the areaMatrix
 */
std::vector<std::unique_ptr<AABB>> createMapAABB(const MapAABBData &data);

/**
 * @brief Recursively creates an AABB tree for a single area.
 * @param aabb The parent AABB. If this is the root AABB, then this is nullptr.
 * @param x The min x coordinate of the area.
 * @param y The min y coordinate of the area.
 * @param size The size of the area in tiles. Width and height of the area are the same.
 */
std::unique_ptr<AABB> createAreaAABB(AABB *aabb, const int x, const int y, const int size, const MapAABBData &input);
