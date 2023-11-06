#include "Utils.hpp"
#include <glm/vec3.hpp>
#include "graphics.h"
#include "RehtiReader.hpp"

Vertex aiVector3DToVertex(const aiVector3D &vector)
{
  float texU = vector.x / Config.AREA_WIDTH;
  float texV = vector.z / Config.AREA_WIDTH;
  return Vertex{glm::vec3(vector.x, vector.y, vector.z), glm::vec3(0.0f), glm::vec2(texU, texV)};
}

std::vector<unsigned int> aiFaceToVector(const aiFace &face)
{
  std::vector<unsigned int> faceVector;
  for (unsigned int i = 0; i < face.mNumIndices; i++)
  {
    faceVector.push_back(face.mIndices[i]);
  }
  return faceVector;
}

std::vector<std::vector<Vertex>> aiVector3DMatrixToVertexVector(const std::vector<std::vector<aiVector3D>> &aiVector3DMatrix)
{
  std::vector<std::vector<Vertex>> vertexVector;
  for (auto &row : aiVector3DMatrix)
  {
    std::vector<Vertex> vertexRow;
    for (auto &col : row)
    {
      vertexRow.push_back(aiVector3DToVertex(col));
    }
    vertexVector.push_back(vertexRow);
  }
  return vertexVector;
}

std::vector<std::vector<uint32_t>> aiFaceMatrixToVector(const std::vector<std::vector<aiFace>> &aiFaceMatrix)
{
  std::vector<std::vector<unsigned int>> faceVector;
  for (auto &row : aiFaceMatrix)
  {
    std::vector<unsigned int> faceRow;
    for (auto &col : row)
    {
      std::vector<unsigned int> face = aiFaceToVector(col);
      faceRow.insert(faceRow.end(), face.begin(), face.end());
    }
    faceVector.push_back(faceRow);
  }
  return faceVector;
}

GameObjectGraphicData createGameObjectGraphicData(const std::vector<aiVector3D> &aiVector3DList, const std::vector<aiFace> &aiFaceList)
{
  GameObjectGraphicData gameObjectGraphicData;

  for (const auto &aiVector : aiVector3DList)
  {
    gameObjectGraphicData.vertices.push_back(aiVector3DToVertex(aiVector));
  }
  for (const auto &aiFace : aiFaceList)
  {
    std::vector<unsigned int> face = aiFaceToVector(aiFace);
    gameObjectGraphicData.indices.insert(gameObjectGraphicData.indices.end(), face.begin(), face.end());
  }

  return gameObjectGraphicData;
}