#include <string>
#include <vector>
#include <cstdint>

#include "Utils.hpp"
#include "MapReader.hpp"
#include "AssimpObjectReader.hpp"
#include "../../Config.hpp"

void loadAccessMap(std::vector<std::vector<uint8_t>> &accessMap, std::string path)
{
  std::vector<std::vector<std::string>> accessMapString;
  readMatrixFromFile(accessMapString, path);

  for (auto &row : accessMapString)
  {
    std::vector<uint8_t> rowUint8;
    for (auto &col : row)
    {
      rowUint8.push_back(std::stoi(col));
    }
    accessMap.push_back(rowUint8);
  }
}

void loadHeightMap(std::vector<std::vector<int>> &heightMap, std::string path)
{
  std::vector<std::vector<std::string>> heightMapString;
  readMatrixFromFile(heightMapString, path);

  for (auto &row : heightMapString)
  {
    std::vector<int> rowInt;
    for (auto &col : row)
    {
      rowInt.push_back(std::stoi(col));
    }
    heightMap.push_back(rowInt);
  }
}

void loadAreaMap(std::vector<std::vector<std::string>> &areaMap, std::string path)
{
  readMatrixFromFile(areaMap, path);
}

void loadAreaMapObjs(std::vector<std::vector<std::string>> &areaMap, std::vector<std::vector<aiVector3D>> &areaVertexList, std::vector<std::vector<aiFace>> &areaFaceList)
{
  for (int i = 0; i < areaMap.size(); i++)
  {
    for (int j = 0; j < areaMap[i].size(); j++)
    {
      const std::string &area = areaMap[i][j];
      const std::string filepath = Config.GENERATED_AREA_OBJ_PATH + area + ".obj";
      std::vector<aiVector3D> vertices;
      std::vector<aiFace> faces;
      loadOBJFile(filepath, vertices, faces);
      areaVertexList.push_back(vertices);
      areaFaceList.push_back(faces);
    }
  }
}