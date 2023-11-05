#include <iostream>

#include "AssimpObjectReader.hpp"

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