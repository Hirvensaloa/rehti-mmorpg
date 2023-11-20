#pragma once

#include <map>
#include <string>
#include <vector>

#include "TextureReader.hpp"
#include "../../Config.hpp"
#include "AssetTypes.hpp"

/**
 * @brief Loads all of the assets into memory (objects, textures, etc.).
 * @note This is a singleton class. Also all the assets are mapped with their filename as the key.
 */
class AssetCache
{
public:
  /**
   * @brief Returns the singleton instance of the AssetCache
   */
  static AssetCache &getInstance()
  {
    static AssetCache instance;
    return instance;
  }

  /**
   * @brief Loads all the assets into memory
   */
  void loadAssets();

  /**
   * @brief Returns the map of area asset data
   */
  std::map<std::string, MapAreaAssetData> &getAreaAssetData();

  /**
   * @brief Returns the map of object asset data
   */
  std::map<std::string, ObjectAssetData> &getObjectAssetData();

  /**
   * @brief Returns the map of character asset data
   */
  std::map<std::string, CharacterAssetData> &getCharacterAssetData();

  /**
   * @brief Returns the map of item asset data
   */
  std::map<std::string, ItemAssetData> &getItemAssetData();

  /**
   * @brief Returns the map of texture asset data
   */
  std::map<std::string, ImageData> &getTextureAssetData();

  /**
   * @brief Get default texture
   * @return ImageData
   */
  ImageData getDefaultTexture();

private:
  AssetCache() {}
  AssetCache(AssetCache const &);
  void operator=(AssetCache const &);

  /**
   * @brief Loads the game object assets data
   *
   * @param gameObjects The game objects
   * @return Map of game object id and its corresponding asset data (ObjectAssetData)
   */
  std::map<std::string, ObjectAssetData> loadGameObjectAssetData(const GameObjects &gameObjects);

  /**
   * @brief Loads the character assets data
   * @return Map of character id and its corresponding asset data (CharacterAssetData)
   */
  std::map<std::string, CharacterAssetData> loadCharacterAssetData();

  /**
   * @brief Loads the map area assets data
   *
   * @param areaMap The area map
   * @return Map of area name and its corresponding asset data (MapAreaAssetData)
   */
  std::map<std::string, MapAreaAssetData> loadAreaAssetData(const std::vector<std::vector<std::string>> &areaMap);

  std::map<std::string, MapAreaAssetData> areaAssetDataM;
  std::map<std::string, ObjectAssetData> objectAssetDataM;
  std::map<std::string, CharacterAssetData> characterAssetDataM;
  std::map<std::string, ItemAssetData> itemAssetDataM;
  std::map<std::string, ImageData> textureAssetDataM;
  ImageData defaultTextureM;
};