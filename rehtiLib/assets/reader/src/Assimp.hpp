#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <vector>

#include "AssetTypes.hpp"

/**
 * @brief Loads the object from the given path.
 * @param path to the object file.
 * @param vertices Empty vector of float that will be filled with the vertices.
 * @param indices Empty vector of unsigned int that will be filled with the indices.
 * @param scalingFactor The scaling factor for the vertices
 * @return True if the object was loaded successfully, false otherwise.
 */
bool loadOBJFile(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& faces, const float scalingFactor = 1.0f);

/**
 * @brief Loads the object from the given path.
 * @param path to the object file.
 * @param vertices Empty vector of float that will be filled with the vertices.
 * @param indices Empty vector of unsigned int that will be filled with the indices.
 * @param animations Empty array of Animation that will be filled with the animations.
 * @param bones Empty vector of BoneNode that will be filled with the bones.
 * @param transformations Empty vector of glm::mat4 that will be filled with the transformations of the bind pose bones.
 * @return True if the object was loaded successfully, false otherwise.
 */
bool loadGlTFFile(const std::string& path, std::vector<CharacterVertex>& vertices, std::vector<uint32_t>& indices, std::array<Animation, ANIMATION_TYPE_COUNT>& animations, std::vector<BoneNode>& bones, std::vector<glm::mat4>& transformations);

/**
 * @brief Loads animations from the given scene.
 * @param scene to load animations from.
 * @param nameToIndex map of bone names to their indices.
 * @param animations array to fill with animations.
 * @return boolean indicating success
 */
size_t loadAnimations(const aiScene* scene, const std::map<std::string, uint32_t> nameToIndex, std::array<Animation, ANIMATION_TYPE_COUNT>& animations);

/**
 * @brief Fills the given vectors with hierarchy and bone data.
 * @param rootNode is the root node of the hierarchy.
 * @param boneList is the list of bones.
 * @param transformations is the list of transformations.
 * @param nameToIndex is the map of bone names to their indices.
 * @return The number of bones.
 */
size_t fillSkeleton(aiNode* rootNode, std::vector<BoneNode>& boneList, std::vector<glm::mat4>& transformations, std::map<std::string, uint32_t>& nameToIndex);

/**
 * @brief Convert aiVector3D to Vertex
 * @return Vertex
 */
Vertex aiVector3DToVertex(const aiVector3D& vector);

/**
 * @brief Convert vector of aiVector3D to vector of Vertex
 * @return Vector of Vertex
 */
std::vector<Vertex> aiVector3DVectorToVertexVector(const std::vector<aiVector3D>& vectors);

/**
 * @brief Convert aiFace to RehtiGraphics Face
 * @return RehtiGraphics Face
 */
std::vector<uint32_t> aiFaceToFace(const aiFace& face);

/**
 * @brief Convert vector of aiFace to vector of RehtiGraphics Faces
 * @return Vector of RehtiGraphics Faces
 */
std::vector<uint32_t> aiFaceVectorToFaceVector(const std::vector<aiFace>& faces);

/**
 * @brief Convert aiMatrix4x4 to glm::mat4
 * @return glm::mat4
 */
glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& matrix);

/**
 * @brief Convert aiQuaternion to glm::quat
 * @return glm::quat
 */
glm::quat aiQuaternionToGlm(const aiQuaternion& quaternion);

/**
 * @brief Convert aiVector3D to glm::vec3
 * @return glm::vec3
 */
glm::vec3 aiVector3DToGlm(const aiVector3D& vector);

/**
 * @brief Prints out faulty vertices
 * @param vertices to check
 */
void debugCharacterVertices(const std::vector<CharacterVertex>& vertices);
