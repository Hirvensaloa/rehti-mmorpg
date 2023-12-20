#include <iostream>

#include "../../Config.hpp"
#include "Assimp.hpp"
#include "ObjectReader.hpp"
#include "TextureReader.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <queue>

constexpr double EPSILON = 0.00001;
constexpr float EPSILON_F = 0.00001f;

#pragma region Inlines

inline float interpolatedTimeFactor(const double now, const double prevTimeFrame, const double frameDiff, const double totalTime)
{
    double diff = now - prevTimeFrame;
    if (diff < 0.0)
    {
        diff += totalTime;
    }
    return diff / frameDiff;
}

inline bool equalD(const double a, const double b)
{
    return std::abs(a - b) < EPSILON;
}

inline bool equalF(const float a, const float b)
{
    return std::abs(a - b) < EPSILON_F;
}

inline glm::vec3 interpolateLinear(const glm::vec3& start, const glm::vec3& end, float timeFactor)
{
    return (1.f - timeFactor) * start + timeFactor * end;
}

inline glm::quat interpolateLinear(const glm::quat& start, const glm::quat& end, float timeFactor)
{
    return glm::slerp(start, end, timeFactor);
}

inline glm::vec3 interpolateAIVectorkeys(const aiVectorKey& current, const aiVectorKey& prev, const double now, const double totalTicks)
{
    double frameDiff = current.mTime - prev.mTime;
    if (frameDiff < 0.0)
    {
        frameDiff += totalTicks;
    }
    float factor = (equalD(frameDiff, 0.0)) ? 1.f : interpolatedTimeFactor(now, prev.mTime, frameDiff, totalTicks);
    return interpolateLinear(aiVector3DToGlm(prev.mValue), aiVector3DToGlm(current.mValue), factor);
}

inline glm::quat interpolateAIQuatkeys(const aiQuatKey& current, const aiQuatKey& prev, const double now, double totalTicks)
{
    double frameDiff = current.mTime - prev.mTime;
    if (frameDiff < 0.0)
    {
        frameDiff += totalTicks;
    }
    float factor = (equalD(frameDiff, 0.0)) ? 1.f : interpolatedTimeFactor(now, prev.mTime, frameDiff, totalTicks);
    return interpolateLinear(aiQuaternionToGlm(prev.mValue), aiQuaternionToGlm(current.mValue), factor);
}

inline bool containsSubString(const std::string& str, const std::string& subStr)
{
    return str.find(subStr) != std::string::npos;
}

#pragma endregion

bool loadOBJFile(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& faces, const float scalingFactor)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FlipWindingOrder);

    if (!scene)
    {
        std::cout << "Failed to load object file: " << path << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        bool hasNormals = mesh->HasNormals();
        bool hasTexCoords = mesh->HasTextureCoords(0);

        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            Vertex vertex{};
            vertex.pos = aiVector3DToGlm(mesh->mVertices[j]) * scalingFactor;
            if (hasNormals)
                vertex.normal = aiVector3DToGlm(mesh->mNormals[j]);
            if (hasTexCoords)
                vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
            vertices.push_back(vertex);
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            std::vector<uint32_t> faceIndices = aiFaceToFace(mesh->mFaces[j]);
            faces.insert(faces.end(), faceIndices.begin(), faceIndices.end());
        }
    }

    return true;
}

/**
 * @brief Loads the object from the given path.
 * @param path to the object file.
 * @param vertices Empty vector of float that will be filled with the vertices.
 * @param indices Empty vector of unsigned int that will be filled with the indices.
 * @param animations Empty array of Animation that will be filled with the animations.
 * @param bones Empty vector of BoneNode that will be filled with the bones.
 * @return True if the object was loaded successfully, false otherwise.
 */
bool loadGlTFFile(const std::string& path, std::vector<CharacterVertex>& vertices, std::vector<uint32_t>& indices, std::array<Animation, ANIMATION_TYPE_COUNT>& animations, std::vector<BoneNode>& bones, std::vector<glm::mat4>& transformations)
{
    Assimp::Importer importer;
    // additionally flip the winding order. Our engine uses clockwise convention, while gltf is ccw
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_FlipWindingOrder);

    if (!scene)
    {
        std::cout << "Failed to load glTF file: " << path << std::endl;
        return false;
    }

    aiNode* rootBone = scene->mRootNode->FindNode("rootbone");
    if (!rootBone)
    {
        std::cout << "Failed to find rootbone in glTF file: " << path << std::endl;
        return false;
    }
    glm::mat4 rootInverseTransform = aiMatrix4x4ToGlm(scene->mRootNode->mTransformation);
    transformations.push_back(rootInverseTransform);

    std::map<std::string, uint32_t> nameToIndex;

    // aiNode* modelNode = scene->mRootNode->FindNode("Model");
    // fill armature / skeleton
    size_t boneCount = fillSkeleton(rootBone, bones, transformations, nameToIndex);
    size_t loadedAnimations = 0;
    if (MAX_BONES < boneCount)
    {
        std::cout << "Warning: " << path << " has more bones than the maximum. " << boneCount << " > " << MAX_BONES << std::endl;
        std::cout << "Not loading animations for this model" << std::endl;
    }
    else
    {
        loadedAnimations = loadAnimations(scene, nameToIndex, animations);
    }
    if (loadedAnimations == 0)
    {
        std::cout << "No animations found for a glTF file: " << path << std::endl;
    }

    // get the mesh
    for (unsigned int meshI = 0; meshI < scene->mNumMeshes; meshI++)
    {
        aiMesh* mesh = scene->mMeshes[meshI];
        bool normals = mesh->HasNormals();
        bool texCoords = mesh->HasTextureCoords(0);
        bool tex1 = mesh->HasTextureCoords(1);

        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            CharacterVertex charVert{};
            charVert.pos = aiVector3DToGlm(mesh->mVertices[j]);
            if (normals)
                charVert.normal = aiVector3DToGlm(mesh->mNormals[j]);
            if (texCoords)
                charVert.texCoord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);

            vertices.push_back(charVert);
        }
        // The following vector tells how many bones are registered per vertex. (max 4)
        std::vector<uint32_t> bonesUsed(vertices.size(), 0);

        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            std::vector<uint32_t> faceIndices = aiFaceToFace(mesh->mFaces[j]);
            indices.insert(indices.end(), faceIndices.begin(), faceIndices.end());
        }
        // bones

        // get bone weights and ids
        for (size_t i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];
            std::string boneName = std::string(bone->mName.C_Str());
            if (!nameToIndex.contains(boneName)) // discard control nodes, etc.
            {
                continue;
            }
            uint32_t index = nameToIndex[boneName];
            size_t numWeights = bone->mNumWeights;
            bones[index].boneOffset = aiMatrix4x4ToGlm(bone->mOffsetMatrix);

            for (size_t j = 0; j < numWeights; j++)
            {
                aiVertexWeight weight = bone->mWeights[j];
                uint32_t vertexIndex = weight.mVertexId;
                float weightValue = weight.mWeight;
                if (equalF(weightValue, 0.f))
                {
                    continue;
                }

                uint32_t currentBone = bonesUsed[vertexIndex];
                if (currentBone < 4)
                {
                    vertices[vertexIndex].boneIDs[currentBone] = index;
                    vertices[vertexIndex].boneWeights[currentBone] = weightValue;
                    bonesUsed[vertexIndex]++;
                }
                else
                {
                    float boneSum = vertices[vertexIndex].boneWeights.x + vertices[vertexIndex].boneWeights.y + vertices[vertexIndex].boneWeights.z + vertices[vertexIndex].boneWeights.w;

                    vertices[vertexIndex].boneWeights /= boneSum;
                    break;
                }
            } // end of weight for
        }     // end of bone for
    }         // end of mesh for
    debugCharacterVertices(vertices);
    return true;
}

size_t loadAnimations(const aiScene* scene, std::map<std::string, uint32_t> nameToIndex, std::array<Animation, ANIMATION_TYPE_COUNT>& animations)
{
    size_t loadedAnimations = 0;
    // for each animation
    for (uint32_t i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* animation = scene->mAnimations[i];
        std::string animName = std::string(animation->mName.C_Str());
        AnimationType animType = getAnimationType(animName);

        if (animType == AnimationType::UNDEFINED || animation->mNumChannels == 0)
        {
            continue;
        }
        Animation newAnimation{};
        uint32_t numBoneChannels = animation->mNumChannels;
        uint32_t maxKeys = std::max({animation->mChannels[0]->mNumPositionKeys,
                                     animation->mChannels[0]->mNumRotationKeys,
                                     animation->mChannels[0]->mNumScalingKeys});
        newAnimation.animationNodes.resize(maxKeys);
        newAnimation.totalTicks = animation->mDuration;
        newAnimation.ticksPerSecond = (0 < animation->mTicksPerSecond) ? animation->mTicksPerSecond : 24;
        newAnimation.duration = animation->mDuration / animation->mTicksPerSecond;
        // for each bone
        for (uint32_t j = 0; j < numBoneChannels; j++)
        {
            aiNodeAnim* animationNode = animation->mChannels[j];
            std::string boneName = std::string(animationNode->mNodeName.C_Str());
            if (!nameToIndex.contains(boneName)) // discard control nodes, etc.
            {
                continue;
            }
            uint32_t index = nameToIndex[boneName];
            uint32_t nSca = animationNode->mNumScalingKeys;
            uint32_t nRot = animationNode->mNumRotationKeys;
            uint32_t nPos = animationNode->mNumPositionKeys;
            uint32_t numKeys = std::max({nSca,
                                         nRot,
                                         nPos});
            uint32_t keySum = animationNode->mNumPositionKeys + animationNode->mNumRotationKeys + animationNode->mNumScalingKeys;
            if (maxKeys < numKeys)
            {
                std::cout << "Warning: " << boneName << " has more keys than the previous maximum. " << numKeys << " > " << maxKeys << std::endl;
                maxKeys = numKeys;
                newAnimation.animationNodes.resize(maxKeys);
            }
            uint32_t si = 0;
            uint32_t ri = 0;
            uint32_t ti = 0;
            bool hasScale = 1 < nSca;
            bool hasRotation = 1 < nRot;
            bool hasPosition = 1 < nPos;
            double totalKeys = std::max({animationNode->mScalingKeys[nSca - 1].mTime,
                                         animationNode->mRotationKeys[nRot - 1].mTime,
                                         animationNode->mPositionKeys[nPos - 1].mTime});
            uint32_t keyIndex = 0;
            // Until all keys are processed for this bone
            while (keyIndex < numKeys)
            {
                uint32_t psi = (nSca + si - 1) % nSca; // previous scale index
                uint32_t pri = (nRot + ri - 1) % nRot; // previous rotation index
                uint32_t pti = (nPos + ti - 1) % nPos; // previous position index
                aiVectorKey sk = animationNode->mScalingKeys[si];
                aiQuatKey rk = animationNode->mRotationKeys[ri];
                aiVectorKey pk = animationNode->mPositionKeys[ti];
                double st = (hasScale) ? sk.mTime : newAnimation.totalTicks;
                double rt = (hasRotation) ? rk.mTime : newAnimation.totalTicks;
                double pt = (hasPosition) ? pk.mTime : newAnimation.totalTicks;
                double now = std::min({st, rt, pt});
                // prev
                aiVectorKey psk = animationNode->mScalingKeys[psi];
                aiQuatKey prk = animationNode->mRotationKeys[pri];
                aiVectorKey ppk = animationNode->mPositionKeys[pti];

                GfxOrientation orientation{};
                orientation.scale = interpolateAIVectorkeys(sk, psk, now, totalKeys);
                orientation.rotation = interpolateAIQuatkeys(rk, prk, now, totalKeys);
                orientation.position = interpolateAIVectorkeys(pk, ppk, now, totalKeys);

                // set the orientation of the bone at keyframe k
                newAnimation.animationNodes[keyIndex].bones[index] = orientation;
                newAnimation.animationNodes[keyIndex].time = now;
                // advance the indices
                if (hasScale && equalD(now, sk.mTime))
                {
                    si++;
                }
                if (hasRotation && equalD(now, rk.mTime))
                {
                    ri++;
                }
                if (hasPosition && equalD(now, pk.mTime))
                {
                    ti++;
                }

                keyIndex++;
            } // for each animation key
        }     // end of bone for
        animations[getAnimIndex(animType)] = newAnimation;
        loadedAnimations++;
    } // end of animation for
    return loadedAnimations;
}

size_t fillSkeleton(aiNode* rootNode, std::vector<BoneNode>& boneList, std::vector<glm::mat4>& transformations, std::map<std::string, uint32_t>& nameToIndex)
{
    // bonecount acts as the next added index as well as count of bones
    size_t boneCount = 0;
    uint32_t level = 0;
    // first bone has no parent
    BoneNode bone{};
    bone.parent = -1;
    boneList.push_back(bone);
    // add name to index map
    std::string rootName = std::string(rootNode->mName.C_Str());
    nameToIndex[rootName] = 0;
    boneCount++;
    std::queue<aiNode*> nodeQueue;
    nodeQueue.push(rootNode);
    // breadth first add
    while (!nodeQueue.empty())
    {
        // deque
        aiNode* currentNode = nodeQueue.front();
        nodeQueue.pop();
        // push current transformation of the bone to the matrix list
        transformations.push_back(aiMatrix4x4ToGlm(currentNode->mTransformation));

        // add children to queue if any
        uint32_t numChildren = currentNode->mNumChildren;
        int parentIndex = nameToIndex[std::string(currentNode->mName.C_Str())];
        if (0 < numChildren)
        {
            for (uint32_t i = 0; i < numChildren; i++)
            {
                // create a bone node
                BoneNode newNode{};
                newNode.parent = parentIndex;
                std::string name = std::string(currentNode->mChildren[i]->mName.C_Str());
                if (containsSubString(name, "Ctrl")) // do not add control nodes
                {
                    continue;
                }
                // set children of parent
                boneList[parentIndex].children.push_back(boneCount);
                // map name to index
                nameToIndex[name] = boneCount;
                // add the bone of the child to the list
                boneList.push_back(newNode);
                // increment bonecount and check children for more bones
                boneCount++;
                nodeQueue.push(currentNode->mChildren[i]);
            }
        }
    }

    return boneCount;
}

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& matrix)
{
    glm::mat4 glmMatrix;
    glmMatrix[0][0] = matrix.a1;
    glmMatrix[0][1] = matrix.b1;
    glmMatrix[0][2] = matrix.c1;
    glmMatrix[0][3] = matrix.d1;
    glmMatrix[1][0] = matrix.a2;
    glmMatrix[1][1] = matrix.b2;
    glmMatrix[1][2] = matrix.c2;
    glmMatrix[1][3] = matrix.d2;
    glmMatrix[2][0] = matrix.a3;
    glmMatrix[2][1] = matrix.b3;
    glmMatrix[2][2] = matrix.c3;
    glmMatrix[2][3] = matrix.d3;
    glmMatrix[3][0] = matrix.a4;
    glmMatrix[3][1] = matrix.b4;
    glmMatrix[3][2] = matrix.c4;
    glmMatrix[3][3] = matrix.d4;
    return glmMatrix;
}

glm::quat aiQuaternionToGlm(const aiQuaternion& quaternion)
{
    return glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
}

glm::vec3 aiVector3DToGlm(const aiVector3D& vector)
{
    return glm::vec3(vector.x, vector.y, vector.z);
}

void debugCharacterVertices(const std::vector<CharacterVertex>& vertices)
{
    uint32_t i = 0;
    uint32_t faultyVertices = 0;
    glm::vec3 mins = glm::vec3(1000000.f);
    glm::vec3 maxs = glm::vec3(-1000000.f);
    for (const auto& vertex : vertices)
    {
        glm::vec4 w = vertex.boneWeights;
        float boneSum = w.x + w.y + w.z + w.w;
        if (0.001f < abs(boneSum - 1.f))
        {
            std::cout << "Vertex " << i << " has faulty bone weights with a sum of " << boneSum << std::endl;
            std::cout << "Bone weights: " << w.x << ", " << w.y << ", " << w.z << ", " << w.w << std::endl;
            std::cout << "Bone IDs: " << vertex.boneIDs.x << ", " << vertex.boneIDs.y << ", " << vertex.boneIDs.z << ", " << vertex.boneIDs.w << std::endl;
            faultyVertices++;
        }
        mins = glm::min(mins, vertex.pos);
        maxs = glm::max(maxs, vertex.pos);
        i++;
    }
    std::cout << "Found " << faultyVertices << " faulty vertices" << std::endl;
    glm::vec3 diff = maxs - mins;
    std::cout << "Model dimensions are: " << diff.x
              << ", " << diff.y << ", " << diff.z << std::endl;
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