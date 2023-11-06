#include "MapLoader.hpp"

struct Vertex
{
  float x, y, z;

  Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

/**
 * @brief Writes a vertex to an OBJ file.
 *
 * @param objFile
 * @param vertex
 */
void writeVertex(std::ofstream &objFile, const Vertex &vertex)
{
  objFile << "v " << vertex.x * Config.TILE_SIDE_SCALE << " " << vertex.y * Config.HEIGHT_MAP_SCALE << " " << vertex.z * Config.TILE_SIDE_SCALE << std::endl;
};

/**
 * @brief Writes a face to an OBJ file.
 *
 * @param objFile
 * @param corner1
 * @param corner2
 * @param corner3
 */
void writeFace(std::ofstream &objFile, const int corner1, const int corner2, const int corner3)
{
  objFile << "f " << corner1 << " " << corner2 << " " << corner3 << std::endl;
};

/**
 * @brief Generates obj map for each area of the map.
 * @param heightMap The height matrix of the map.
 * @param areaMap The map of all the areas.
 */
void generateAreaObjs(const std::vector<std::vector<int>> &heightMap, const std::vector<std::vector<std::string>> &areaMap)
{
  for (int i = 0; i < areaMap.size(); i++)
  {
    for (int j = 0; j < areaMap[i].size(); j++)
    {
      const std::string area = areaMap[i][j];
      std::cout << "Generating area " << area << i << j << std::endl;
      const std::string filename = area + std::to_string(i) + std::to_string(j) + ".obj";
      // Create an OBJ file for writing
      std::ofstream objFile(Config.GENERATED_AREA_OBJ_PATH + filename);

      if (!objFile.is_open())
      {
        throw std::runtime_error("Could not open " + area + ".obj for writing");
      }

      int vertexCount = 0;
      for (int row = i * Config.AREA_WIDTH; row < i * Config.AREA_WIDTH + Config.AREA_WIDTH; row++)
      {
        for (int col = j * Config.AREA_HEIGHT; col < j * Config.AREA_HEIGHT + Config.AREA_HEIGHT; col++)
        {
          // A cell is divided into inner square which is at the height of the cell and surrounding the inner square are 4 trapezoids which are at the middle of the cell's and it's neighbours height.
          // The trapezoids are further broken down into two corners and a center rectancle. The trapezoids are used to create a "SSSmooth" transition between the cells.

          float innerSquareRadius = Config.TILE_SIDE_UNIT / 4.0;
          float squareRadius = Config.TILE_SIDE_UNIT / 2.0;
          // The inner square
          writeVertex(objFile, Vertex(col + innerSquareRadius, heightMap[row][col], row - innerSquareRadius)); // Top right
          writeVertex(objFile, Vertex(col + innerSquareRadius, heightMap[row][col], row + innerSquareRadius)); // Bottom right
          writeVertex(objFile, Vertex(col - innerSquareRadius, heightMap[row][col], row + innerSquareRadius)); // Bottom left
          writeVertex(objFile, Vertex(col - innerSquareRadius, heightMap[row][col], row - innerSquareRadius)); // Top left
          int innerTopRight = 1;
          int innerBottomRight = 2;
          int innerBottomLeft = 3;
          int innerTopLeft = 4;

          // Next the trapezoids, two of the vertices are shared with the inner square

          TileHeight tileHeight = calculateTileHeights(heightMap, row, col);
          // The bottom trapezoid
          writeVertex(objFile, Vertex(col + squareRadius, tileHeight.bottomRight, row + squareRadius)); // Bottom right
          writeVertex(objFile, Vertex(col - squareRadius, tileHeight.bottomLeft, row + squareRadius));  // Bottom left
          writeVertex(objFile, Vertex(col - innerSquareRadius, tileHeight.bottom, row + squareRadius)); // Bottom center left
          writeVertex(objFile, Vertex(col + innerSquareRadius, tileHeight.bottom, row + squareRadius)); // Bottom center right
          int bottomRight = 5;
          int bottomLeft = 6;
          int bottomCenterLeft = 7;
          int bottomCenterRight = 8;

          // The left trapezoid
          writeVertex(objFile, Vertex(col - squareRadius, tileHeight.topLeft, row - squareRadius));   // Left top
          writeVertex(objFile, Vertex(col - squareRadius, tileHeight.left, row - innerSquareRadius)); // Left center top
          writeVertex(objFile, Vertex(col - squareRadius, tileHeight.left, row + innerSquareRadius)); // Left center bottom
          int leftTop = 9;
          int leftCenterTop = 10;
          int leftCenterBottom = 11;

          // The top trapezoid
          writeVertex(objFile, Vertex(col + squareRadius, tileHeight.topRight, row - squareRadius)); // Top right
          writeVertex(objFile, Vertex(col - innerSquareRadius, tileHeight.top, row - squareRadius)); // Top center left
          writeVertex(objFile, Vertex(col + innerSquareRadius, tileHeight.top, row - squareRadius)); // Top center right
          int topRight = 12;
          int topCenterLeft = 13;
          int topCenterRight = 14;

          // The right trapezoid
          writeVertex(objFile, Vertex(col + squareRadius, tileHeight.right, row - innerSquareRadius)); // Right center top
          writeVertex(objFile, Vertex(col + squareRadius, tileHeight.right, row + innerSquareRadius)); // Right center bottom
          int rightCenterTop = 15;
          int rightCenterBottom = 16;

          // Define and write the triangles
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

          // Keep track of the vertex index
          vertexCount += 16;
        }
      }

      objFile.close();
    }
  }

  std::cout << "Map object file generated" << std::endl;
};