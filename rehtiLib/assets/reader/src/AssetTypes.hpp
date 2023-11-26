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
};

/**
 * @brief Contains asset data loaded into memory for a specific character
 */
struct CharacterAssetData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    ImageData texture;
};

/**
 * @brief Contains asset data loaded into memory for a specific item
 */
struct ItemAssetData
{
    // TODO: Add when items have icons defined
    // StbImageData icon;
    // TODO: Add when items have object files defined
    // std::vector<aiVector3D> vertices;
    // std::vector<aiFace> faces;
};