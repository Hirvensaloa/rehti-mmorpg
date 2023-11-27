#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

#include "../../../graphics/src/BasicTypes.hpp"
#include "../../Config.hpp"

/**
 * @file Contains all the asset related types
 */

 /**
  * @brief Contains asset data loaded into memory for a specific area
  */
struct MapAreaAssetData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<ImageData> textures;
	ImageData blendMap;
};

/**
 * @brief Contains asset data loaded into memory for a specific object
 */
struct ObjectAssetData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	ImageData texture;
	std::string characterInteractAnimation;
};

/**
 * @brief Contains asset data loaded into memory for a specific character
 */
struct CharacterAssetData
{
	std::vector<CharacterVertex> vertices;
	std::vector<uint32_t> indices;
	ImageData texture;
	std::vector<glm::mat4> boneTransformations{}; ///< bone transformation storage data
	std::vector<BoneNode> bones;
	std::array<Animation, ANIMATION_TYPE_COUNT> animations;
};

/**
 * @brief Contains asset data loaded into memory for a specific item
 */
struct ItemAssetData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	ImageData texture;
	ImageData icon;
};