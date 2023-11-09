#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>

/**
 * @brief Loads the object from the given path.
 * @param path to the object file.
 * @param vertices Empty vector of float that will be filled with the vertices.
 * @param indices Empty vector of unsigned int that will be filled with the indices.
 * @param scalingFactor The scaling factor for the vertices
 */
bool loadOBJFile(const std::string &path, std::vector<aiVector3D> &vertices, std::vector<aiFace> &faces, const float scalingFactor = 1.0f);

/**
 * @brief Reads the area map obj files
 * @param areaMap Contains all the area names and their corresponding positions
 * @param areaVertexList Empty list where the area vertices are loaded
 * @param areaFaceList Empty list where the area faces are loaded
 */
void loadAreaMapObjs(std::vector<std::vector<std::string>> &areaMap, std::vector<std::vector<aiVector3D>> &areaVertexList, std::vector<std::vector<aiFace>> &areaFaceList);

struct GameObjectObjData
{
  std::vector<aiVector3D> vertices;
  std::vector<aiFace> faces;
};

/**
 * @brief Loads the game object obj files
 * @return Map of game object id and its corresponding vertices and faces (GameObjectObjData)
 */
std::map<std::string, GameObjectObjData> loadGameObjectObjs();