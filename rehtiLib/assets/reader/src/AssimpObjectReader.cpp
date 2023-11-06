#include <iostream>

#include "AssimpObjectReader.hpp"
#include "../../Config.hpp"

bool loadOBJFile(const std::string &path, std::vector<aiVector3D> &vertices, std::vector<aiFace> &faces)
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