#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../../Config.hpp"
#include "AssetTypes.hpp"
#include "TextureReader.hpp"

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
    static AssetCache& getInstance()
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
     * @return Map of area name and its corresponding asset data (MapAreaAssetData)
     */
    std::map<std::string, MapAreaAssetData>& getAreaAssetData();

    /**
     * @brief Returns the object asset data by type id
     * @param id object type id
     * @return ObjectAssetData
     */
    const ObjectAssetData& getObjectAssetDataById(int id);

    /**
     * @brief Returns the character asset data by type id
     * @param id character type id
     * @return CharacterAssetData
     */
    const CharacterAssetData& getCharacterAssetDataById(int id);

    /**
     * @brief Returns the item asset data by type id
     * @param id item type id
     * @return ItemAssetData
     */
    const ItemAssetData& getItemAssetDataById(int id);

    /**
     * @brief Returns the map of texture asset data
     * @return Map of texture filename and its corresponding asset data (ImageData)
     */
    std::map<std::string, ImageData>& getTextureAssetData();

    /**
     * @brief Get default texture
     * @return ImageData
     */
    ImageData getDefaultTexture();

    /**
     * @brief Get item asset data map
     * @return std::map<int, ItemAssetData>&
     */
    std::map<int, ItemAssetData>& getItemAssetData();

private:
    AssetCache() {}
    AssetCache(AssetCache const&);
    void operator=(AssetCache const&);

    /**
     * @brief Loads the game object assets data
     *
     * @param gameObjects The game objects
     * @return Map of game object id and its corresponding asset data (ObjectAssetData)
     */
    std::map<int, ObjectAssetData> loadGameObjectAssetData(const GameObjects& gameObjects);

    /**
     * @brief Loads the character assets data
     * @param gameCharacters The game characters data
     * @return Map of character id and its corresponding asset data (CharacterAssetData)
     */
    std::map<int, CharacterAssetData> loadCharacterAssetData(const GameCharacters& gameCharacters);

    /**
     * @brief Loads the map area assets data
     *
     * @param areaMap The area map
     * @return Map of area name and its corresponding asset data (MapAreaAssetData )
     */
    std::map<std::string, MapAreaAssetData> loadAreaAssetData(const std::vector<std::vector<std::string>>& areaMap);

    /**
     * @brief Loads the item assets data
     * @param gameItems GameItems
     * @return Map of item id and its corresponding asset data (ItemAssetData)
     */
    std::map<int, ItemAssetData> loadItemAssetData(const GameItems& gameItems);

    std::map<std::string, MapAreaAssetData> areaAssetDataM;
    std::map<int, ObjectAssetData> objectAssetDataM;
    std::map<int, CharacterAssetData> characterAssetDataM;
    std::map<int, ItemAssetData> itemAssetDataM;
    std::map<std::string, ImageData> textureAssetDataM;
    ImageData defaultTextureM;
};