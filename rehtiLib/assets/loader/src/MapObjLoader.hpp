#include "MapLoader.hpp"

struct Vertex
{
  float x, y, z;

  Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Function to write a vertex to an OBJ file
void writeVertex(std::ofstream &objFile, const Vertex &vertex)
{
  objFile << "v " << vertex.x * Config.TILE_SIDE_SCALE << " " << vertex.y * Config.HEIGHT_MAP_SCALE << " " << vertex.z * Config.TILE_SIDE_SCALE << std::endl;
};

// Function to write a face to an OBJ file. ADD THE CORNERS IN CLOCKWISE DIRECTION.
void writeFace(std::ofstream &objFile, const int corner1, const int corner2, const int corner3)
{
  objFile << "f " << corner1 << " " << corner2 << " " << corner3 << std::endl;
};

struct TileHeight
{
  float bottomRight, bottomLeft, topLeft, topRight, bottom, left, top, right;
};

// Calculate the tile heights, for each corner and then for each side.
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

// Generates the map object file. The file is in OBJ format. The file is built from the height map.
void generateMapObj(const std::vector<std::vector<int>> &heightMap)
{
  // Create an OBJ file for writing
  std::ofstream objFile(Config.GENERATED_MAP_PATH);

  if (!objFile.is_open())
  {
    throw std::runtime_error("Could not open map.obj for writing");
  }

  for (int row = 0; row < heightMap.size(); row++)
  {
    for (int col = 0; col < heightMap[row].size(); col++)
    {
      // A cell is divided into inner square which is at the height of the cell and surrounding the inner square are 4 trapezoids which are at the middle of the cell's and it's neighbours height.
      // The trapezoids are further broken down into two corners and a center rectancle. The trapezoids are used to create a "SSSmooth" transition between the cells.

      // The inner square
      writeVertex(objFile, Vertex(col + 0.25, heightMap[row][col], row - 0.25)); // Top right
      writeVertex(objFile, Vertex(col + 0.25, heightMap[row][col], row + 0.25)); // Bottom right
      writeVertex(objFile, Vertex(col - 0.25, heightMap[row][col], row + 0.25)); // Bottom left
      writeVertex(objFile, Vertex(col - 0.25, heightMap[row][col], row - 0.25)); // Top left
      int innerTopRight = 1;
      int innerBottomRight = 2;
      int innerBottomLeft = 3;
      int innerTopLeft = 4;

      // Next the trapezoids, two of the vertices are shared with the inner square

      TileHeight tileHeight = calculateTileHeights(heightMap, row, col);
      // The bottom trapezoid
      writeVertex(objFile, Vertex(col + 0.5, tileHeight.bottomRight, row + 0.5)); // Bottom right
      writeVertex(objFile, Vertex(col - 0.5, tileHeight.bottomLeft, row + 0.5));  // Bottom left
      writeVertex(objFile, Vertex(col - 0.25, tileHeight.bottom, row + 0.5));     // Bottom center left
      writeVertex(objFile, Vertex(col + 0.25, tileHeight.bottom, row + 0.5));     // Bottom center right
      int bottomRight = 5;
      int bottomLeft = 6;
      int bottomCenterLeft = 7;
      int bottomCenterRight = 8;

      // The left trapezoid
      writeVertex(objFile, Vertex(col - 0.5, tileHeight.topLeft, row - 0.5)); // Left top
      writeVertex(objFile, Vertex(col - 0.5, tileHeight.left, row - 0.25));   // Left center top
      writeVertex(objFile, Vertex(col - 0.5, tileHeight.left, row + 0.25));   // Left center bottom
      int leftTop = 9;
      int leftCenterTop = 10;
      int leftCenterBottom = 11;

      // The top trapezoid
      writeVertex(objFile, Vertex(col + 0.5, tileHeight.topRight, row - 0.5)); // Top right
      writeVertex(objFile, Vertex(col - 0.25, tileHeight.top, row - 0.5));     // Top center left
      writeVertex(objFile, Vertex(col + 0.25, tileHeight.top, row - 0.5));     // Top center right
      int topRight = 12;
      int topCenterLeft = 13;
      int topCenterRight = 14;

      // The right trapezoid
      writeVertex(objFile, Vertex(col + 0.5, tileHeight.right, row - 0.25)); // Right center top
      writeVertex(objFile, Vertex(col + 0.5, tileHeight.right, row + 0.25)); // Right center bottom
      int rightCenterTop = 15;
      int rightCenterBottom = 16;

      // Define and write the triangles
      int vertexCount = 16 * (row * heightMap[row].size() + col);

      // The inner square
      writeFace(objFile, vertexCount + 1, vertexCount + 2, vertexCount + 3);
      writeFace(objFile, vertexCount + 3, vertexCount + 4, vertexCount + 1);

      // The bottom trapezoid
      writeFace(objFile, vertexCount + bottomRight, vertexCount + bottomCenterRight, vertexCount + innerBottomRight);
      writeFace(objFile, vertexCount + bottomCenterRight, vertexCount + bottomCenterLeft, vertexCount + innerBottomLeft);
      writeFace(objFile, vertexCount + innerBottomLeft, vertexCount + innerBottomRight, vertexCount + bottomCenterRight);
      writeFace(objFile, vertexCount + bottomCenterLeft, vertexCount + bottomLeft, vertexCount + innerBottomLeft);

      // The left trapezoid
      writeFace(objFile, vertexCount + leftTop, vertexCount + innerTopLeft, vertexCount + leftCenterTop);
      writeFace(objFile, vertexCount + leftCenterTop, vertexCount + innerTopLeft, vertexCount + leftCenterBottom);
      writeFace(objFile, vertexCount + leftCenterBottom, vertexCount + innerTopLeft, vertexCount + innerBottomLeft);
      writeFace(objFile, vertexCount + innerBottomLeft, vertexCount + bottomLeft, vertexCount + leftCenterBottom);

      // The top trapezoid
      writeFace(objFile, vertexCount + topRight, vertexCount + innerTopRight, vertexCount + topCenterRight);
      writeFace(objFile, vertexCount + topCenterRight, vertexCount + innerTopRight, vertexCount + topCenterLeft);
      writeFace(objFile, vertexCount + topCenterLeft, vertexCount + innerTopRight, vertexCount + innerTopLeft);
      writeFace(objFile, vertexCount + innerTopLeft, vertexCount + leftTop, vertexCount + topCenterLeft);

      // The right trapezoid
      writeFace(objFile, vertexCount + rightCenterTop, vertexCount + innerBottomRight, vertexCount + innerTopRight);
      writeFace(objFile, vertexCount + innerTopRight, vertexCount + topRight, vertexCount + rightCenterTop);
      writeFace(objFile, vertexCount + rightCenterBottom, vertexCount + innerBottomRight, vertexCount + rightCenterTop);
      writeFace(objFile, vertexCount + innerBottomRight, vertexCount + rightCenterBottom, vertexCount + bottomRight);
    }
  }

  objFile.close();

  std::cout << "Map object file generated" << std::endl;
};