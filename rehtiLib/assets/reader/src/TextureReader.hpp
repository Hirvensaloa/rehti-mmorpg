#pragma once

#include <map>
#include <string>

#include "ObjectReader.hpp"
#include "AssetTypes.hpp"

/**
 * @file Reads the texture files
 */

/**
 * @brief Reads all the object's textures + character textures into a map (key=filename, value=ImageData)
 *
 * @param gameObjects GameObjects
 * @return Map of texture filename and its corresponding ImageData
 */
std::map<std::string, ImageData> loadObjectTextures(const GameObjects &gameObjects);

/**
 * @brief Reads all the character's textures into a map (key=filename, value=ImageData)
 *
 * @return Map of texture filename and its corresponding ImageData
 */
std::map<std::string, ImageData> loadCharacterTextures();

/**
 * @brief Reads all different the map texture files
 *
 * @return Map of textures (key=textureId, value=ImageData)
 */
std::map<int, ImageData> loadMapTextures();

/**
 * @brief Loads map texture positions. E.g. which tile contains which texture.
 *
 * @return Matrix of texture positions. Position is indicated by the row and column. The texture id on that position is the cell value.
 */
std::vector<std::vector<int>> loadMapTexturePositions();

/**
 * @brief Creates a blend map for one area. Blend map shows which texture is used on each tile.
 * @note Supports only 5 different textures per area
 *
 * @param mapTexturePositionMatrix Matrix of texture positions with the corresponding texture id
 * @param columnOffset Areas column offset in the mapTexturePositionMatrix
 * @param rowOffset Areas row offset in the mapTexturePositionMatrix
 *
 * @return Blend map image data
 */
ImageData createAreaBlendMap(const std::vector<std::vector<int>> &mapTexturePositionMatrix, int columnOffset, int rowOffset);

/**
 * @brief Loads default texture into memory
 *
 * @return ImageData of the default texture
 */
ImageData loadDefaultTexture();