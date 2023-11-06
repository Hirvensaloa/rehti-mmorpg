#pragma once

#include <glm/vec3.hpp>
#include "graphics.h"
#include "RehtiReader.hpp"

/**
 * @brief Convert aiVector3D to Vertex
 * @return Vertex
 */
Vertex aiVector3DToVertex(const aiVector3D &vector);

/**
 * @brief Convert aiFace to std::vector<unsigned int>
 * @return std::vector<unsigned int>
 */
std::vector<unsigned int> aiFaceToVector(const aiFace &face);

/**
 * @brief Convert matrix of aiVector3D to std::vector<std::vector<Vertex>>
 * @param aiVector3DMatrix Matrix of aiVector3D
 * @return std::vector<std::vector<Vertex>>
 */
std::vector<std::vector<Vertex>> aiVector3DMatrixToVertexVector(const std::vector<std::vector<aiVector3D>> &aiVector3DMatrix);

/**
 * @brief Convert matrix of aiFace to std::vector<std::vector<unsigned int>>
 * @param aiFaceMatrix Matrix of aiFace
 * @return std::vector<std::vector<unsigned int>>
 */
std::vector<std::vector<uint32_t>> aiFaceMatrixToVector(const std::vector<std::vector<aiFace>> &aiFaceMatrix);

/**
 * @brief Contains data from which it can be drawn by the graphics lib
 */
struct GameObjectGraphicData
{
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  ImageData texture = TestValues::GetTestTexture();
};

/**
 * @brief Creates GameObjectGraphicData from assimp data structures
 * @param aiVector3DList list of assimp aiVector3D vertices
 * @param aiFaceList list of assimp aiFace indices
 * @return GameObjectGraphicData
 */
GameObjectGraphicData createGameObjectGraphicData(const std::vector<aiVector3D> &aiVector3DList, const std::vector<aiFace> &aiFaceList);

/**
 * @brief Convert StbImageData to ImageData
 * @return ImageData
 */
ImageData stbImageDataToImageData(const StbImageData &stbImageData);