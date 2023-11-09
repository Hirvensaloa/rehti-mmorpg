#include <iostream>

#include "AssimpObjectReader.hpp"
#include "ObjectReader.hpp"
#include "../../Config.hpp"

bool loadOBJFile(const std::string &path, std::vector<aiVector3D> &vertices, std::vector<aiFace> &faces, const float scalingFactor)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

  if (!scene)
  {
    std::cout << "Failed to load object file: " << path << std::endl;
    return false;
  }

  for (unsigned int i = 0; i < scene->mNumMeshes; i++)
  {
    aiMesh *mesh = scene->mMeshes[i];
    for (unsigned int j = 0; j < mesh->mNumVertices; j++)
    {
      mesh->mVertices[j].x *= scalingFactor;
      mesh->mVertices[j].y *= scalingFactor;
      mesh->mVertices[j].z *= scalingFactor;
      vertices.push_back(mesh->mVertices[j]);
    }
    for (unsigned int j = 0; j < mesh->mNumFaces; j++)
    {
      faces.push_back(mesh->mFaces[j]);
    }
  }

  return true;
}

void loadAreaMapObjs(std::vector<std::vector<std::string>> &areaMap, std::vector<std::vector<aiVector3D>> &areaVertexList, std::vector<std::vector<aiFace>> &areaFaceList)
{
  for (int i = 0; i < areaMap.size(); i++)
  {
    for (int j = 0; j < areaMap[i].size(); j++)
    {
      const std::string &area = areaMap[i][j];
      const std::string filename = area + std::to_string(i) + std::to_string(j) + ".obj";
      const std::string filepath = Config.GENERATED_AREA_OBJ_PATH + filename;
      std::vector<aiVector3D> vertices;
      std::vector<aiFace> faces;
      loadOBJFile(filepath, vertices, faces);
      areaVertexList.push_back(vertices);
      areaFaceList.push_back(faces);
    }
  }
}

std::map<std::string, GameObjectObjData> loadGameObjectObjs()
{
  auto items = fetchItems();
  auto skills = fetchSkills();
  const GameObjects gameObjects = fetchObjects(items, skills);
  std::map<std::string, GameObjectObjData> gameObjectObjDataMap;
  for (const auto &objectId : gameObjects.getObjectIds())
  {
    const std::string idStr = std::to_string(objectId);
    const std::string &filename = idStr + ".obj";
    const std::string filepath = Config.OBJECT_OBJ_PATH + filename;
    std::vector<aiVector3D> vertices;
    std::vector<aiFace> faces;
    bool success = loadOBJFile(filepath, vertices, faces);
    if (success)
    {
      gameObjectObjDataMap[idStr] = {vertices, faces};
    }
  }

  // Load ukko.obj
  std::vector<aiVector3D> vertices;
  std::vector<aiFace> faces;
  bool ukkoSuccess = loadOBJFile(Config.OBJECT_OBJ_PATH + "ukko.obj", vertices, faces, 0.05f);
  if (ukkoSuccess)
  {
    gameObjectObjDataMap["ukko"] = {vertices, faces};
  }

  // Load goblin.obj
  std::vector<aiVector3D> verticesGoblin;
  std::vector<aiFace> facesGoblin;
  bool goblinSuccess = loadOBJFile(Config.OBJECT_OBJ_PATH + "orc1.obj", verticesGoblin, facesGoblin, 0.01f);
  if (goblinSuccess)
  {
    gameObjectObjDataMap["orc1"] = {verticesGoblin, facesGoblin};
  }

  return gameObjectObjDataMap;
}
