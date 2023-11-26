#include <iostream>

#include "../../Config.hpp"
#include "Assimp.hpp"
#include "ObjectReader.hpp"
#include "TextureReader.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <queue>

bool loadOBJFile(const std::string& path, std::vector<aiVector3D>& vertices, std::vector<aiFace>& faces, const float scalingFactor)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene)
    {
        std::cout << "Failed to load object file: " << path << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
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

size_t fillSkeleton(aiNode* rootNode, std::vector<BoneNode>& boneList, std::vector<glm::mat4> transformations, std::map<aiString, uint32_t>& nameToIndex)
{
    // bonecount acts as the next added index as well as count of bones
    size_t boneCount = 0;
    uint32_t level = 0;

    BoneNode bone{};
    bone.parent = -1;
    boneList.push_back(bone);
    uint32_t parentIndex = 0;
    boneCount++;
    std::queue<aiNode*> nodeQueue;
    nodeQueue.push(rootNode);
    // breadth first add
    while (nodeQueue.empty())
    {
        // deque
        aiNode* currentNode = nodeQueue.front();
        nodeQueue.pop();

        uint32_t numChildren = currentNode->mNumChildren;
        nameToIndex[currentNode->mName] = boneCount;
        if (0 < numChildren)
        {
            for (uint32_t i = 0; i < numChildren; i++)
            {
                BoneNode newNode{};
                newNode.parent = parentIndex;
                boneList[parentIndex].children.push_back(boneCount);
                boneCount++;
                nodeQueue.push(currentNode->mChildren[i]);
            }
        }
        parentIndex++;
    }

    return boneCount;
}

Vertex aiVector3DToVertex(const aiVector3D& vector)
{
    float texU = vector.x / Config.AREA_WIDTH;
    float texV = vector.z / Config.AREA_HEIGHT;
    return Vertex{glm::vec3(vector.x, vector.y, vector.z), glm::vec3(0.0f), glm::vec2(texU, texV)};
}

std::vector<Vertex> aiVector3DVectorToVertexVector(const std::vector<aiVector3D>& vectors)
{
    std::vector<Vertex> vertices;
    for (auto& vector : vectors)
    {
        vertices.push_back(aiVector3DToVertex(vector));
    }
    return vertices;
}

std::vector<uint32_t> aiFaceToFace(const aiFace& face)
{
    std::vector<uint32_t> faceVector;
    for (unsigned int i = 0; i < face.mNumIndices; i++)
    {
        faceVector.push_back(face.mIndices[i]);
    }
    return faceVector;
}

std::vector<uint32_t> aiFaceVectorToFaceVector(const std::vector<aiFace>& faces)
{
    std::vector<uint32_t> faceVector;
    for (auto& face : faces)
    {
        std::vector<uint32_t> faceIndices = aiFaceToFace(face);
        faceVector.insert(faceVector.end(), faceIndices.begin(), faceIndices.end());
    }
    return faceVector;
}