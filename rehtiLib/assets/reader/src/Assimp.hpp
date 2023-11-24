#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>

#include "AssetTypes.hpp"

/**
 * @brief Loads the object from the given path.
 * @param path to the object file.
 * @param vertices Empty vector of float that will be filled with the vertices.
 * @param indices Empty vector of unsigned int that will be filled with the indices.
 * @param scalingFactor The scaling factor for the vertices
 */
bool loadOBJFile(const std::string &path, std::vector<aiVector3D> &vertices, std::vector<aiFace> &faces, const float scalingFactor = 1.0f);

/**
 * @brief Convert aiVector3D to Vertex
 * @return Vertex
 */
Vertex aiVector3DToVertex(const aiVector3D &vector);

/**
 * @brief Convert vector of aiVector3D to vector of Vertex
 * @return Vector of Vertex
 */
std::vector<Vertex> aiVector3DVectorToVertexVector(const std::vector<aiVector3D> &vectors);

/**
 * @brief Convert aiFace to RehtiGraphics Face
 * @return RehtiGraphics Face
 */
std::vector<uint32_t> aiFaceToFace(const aiFace &face);

/**
 * @brief Convert vector of aiFace to vector of RehtiGraphics Faces
 * @return Vector of RehtiGraphics Faces
 */
std::vector<uint32_t> aiFaceVectorToFaceVector(const std::vector<aiFace> &faces);
