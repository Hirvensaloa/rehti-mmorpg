#include <set>

#include "AssetCache.hpp"
#include "Assimp.hpp"
#include "CharacterReader.hpp"
#include "MapReader.hpp"

void AssetCache::loadAssets()
{
    std::cout << "ASSET CACHE - Loading assets..." << std::endl;

    // Load default texture
    defaultTextureM = loadDefaultTexture();

    // Load texture files into memory
    auto gameItems = fetchItems();
    auto gameSkills = fetchSkills();
    const auto gameObjects = fetchObjects(gameItems, gameSkills);
    const auto areaMatrix = fetchAreaMatrix();
    const auto gameCharacters = readCharacterData();

    objectAssetDataM = loadGameObjectAssetData(gameObjects);
    std::cout << "ASSET CACHE - Object assets loaded" << std::endl;
    characterAssetDataM = loadCharacterAssetData(gameCharacters);
    std::cout << "ASSET CACHE - Character assets loaded" << std::endl;
    areaAssetDataM = loadAreaAssetData(areaMatrix);
    std::cout << "ASSET CACHE - Area assets loaded" << std::endl;
    itemAssetDataM = loadItemAssetData(gameItems);
    std::cout << "ASSET CACHE - Item assets loaded" << std::endl;

    std::cout << "ASSET CACHE - Assets loaded" << std::endl;
}

std::map<std::string, MapAreaAssetData>& AssetCache::getAreaAssetData()
{
    return areaAssetDataM;
}

std::map<int, ItemAssetData>& AssetCache::getItemAssetData()
{
    return itemAssetDataM;
}

const ObjectAssetData& AssetCache::getObjectAssetDataById(int id)
{
    return objectAssetDataM.at(id);
}

const CharacterAssetData& AssetCache::getCharacterAssetDataById(int id)
{
    return characterAssetDataM.at(id);
}

const ItemAssetData& AssetCache::getItemAssetDataById(int id)
{
    return itemAssetDataM.at(id);
}

std::map<std::string, ImageData>& AssetCache::getTextureAssetData()
{
    return textureAssetDataM;
}

struct ObjectWithFileinfo
{
    int id;
    std::string objFilename;
    std::string textureFilename;
    std::string characterInteractAnimation;
};

std::map<int, ObjectAssetData> AssetCache::loadGameObjectAssetData(const GameObjects& gameObjects)
{
    // Load object textures first
    std::map<std::string, ImageData> textureDataMap = loadObjectTextures(gameObjects);

    std::map<int, ObjectAssetData> objectAssetDataMap;

    std::vector<ObjectWithFileinfo> objectsWithFilenames;

    for (const auto& object : gameObjects.generalObjects)
    {
        objectsWithFilenames.push_back({object.first, object.second.objFilename, object.second.textureFilename});
    }
    for (const auto& object : gameObjects.resourceObjects)
    {
        objectsWithFilenames.push_back({object.first, object.second.objFilename, object.second.textureFilename, object.second.characterInteractAnimation});
    }
    for (const auto& object : gameObjects.lootObjects)
    {
        objectsWithFilenames.push_back({object.first, object.second.objFilename, object.second.textureFilename, object.second.characterInteractAnimation});
    }

    for (const auto& object : objectsWithFilenames)
    {

        const std::string objFilepath = Config.OBJECT_OBJ_PATH + object.objFilename;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> faces;
        bool success = false;
        if (object.objFilename == "defaultmodel")
        {
            success = loadOBJTile(vertices, faces);
        }
        else
        {
            success = loadOBJFile(objFilepath, vertices, faces);
        }

        if (success)
        {
            objectAssetDataMap[object.id] = {vertices, faces, textureDataMap[object.textureFilename], object.characterInteractAnimation};
        }
    }

    return objectAssetDataMap;
}

std::map<int, CharacterAssetData> AssetCache::loadCharacterAssetData(const GameCharacters& gameCharacters)
{
    // Load character textures first
    std::map<std::string, ImageData> textureDataMap = loadCharacterTextures(gameCharacters);

    std::map<int, CharacterAssetData> characterAssetDataMap;
    { // player scope
        PlayerData player = gameCharacters.player;
        const std::string filepath = Config.CHARACTER_GLTF_PATH + player.glTFFilename;
        std::vector<CharacterVertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<BoneNode> bones;
        std::vector<glm::mat4> transformations;
        std::array<Animation, ANIMATION_TYPE_COUNT> animations;
        bool success = loadGlTFFile(filepath, vertices, indices, animations, bones, transformations);
        if (success)
        {
            CharacterAssetData assetData = {vertices, indices, textureDataMap[player.textureFilename], transformations, bones, animations};
            characterAssetDataMap[player.id] = assetData;
        }
    }

    for (const NPCData& npc : gameCharacters.npcs)
    {
        const std::string filepath = Config.CHARACTER_GLTF_PATH + npc.glTFFilename;
        std::vector<CharacterVertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<BoneNode> bones;
        std::vector<glm::mat4> transformations;
        std::array<Animation, ANIMATION_TYPE_COUNT> animations;
        bool success = loadGlTFFile(filepath, vertices, indices, animations, bones, transformations);
        if (success)
        {
            CharacterAssetData assetData = {vertices, indices, textureDataMap[npc.textureFilename], transformations, bones, animations};
            characterAssetDataMap[npc.id] = assetData;
        }
    }

    return characterAssetDataMap;
}

std::map<std::string, MapAreaAssetData> AssetCache::loadAreaAssetData(const std::vector<std::vector<std::string>>& areaMap)
{
    // Load map textures first
    std::map<int, ImageData> textureDataMap = loadMapTextures();
    std::cout << "ASSET CACHE - Map textures loaded" << std::endl;
    std::vector<std::vector<int>> texturePositionMatrix = loadMapTexturePositions();
    std::cout << "ASSET CACHE - Map texture positions loaded" << std::endl;

    std::map<std::string, MapAreaAssetData> mapAreaAssetDataMap;

    for (int i = 0; i < areaMap.size(); i++)
    {
        for (int j = 0; j < areaMap[i].size(); j++)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> faces;
            const std::string areaName = areaMap[i][j];
            const std::string key = areaName + std::to_string(i) + std::to_string(j);

            std::cout << "ASSET CACHE - Loading area assets: " << areaName << std::endl;
            bool success = loadOBJFile(Config.GENERATED_AREA_OBJ_PATH + key + ".obj", vertices, faces);
            if (success)
            {
                const ImageData blendMap = createAreaBlendMap(texturePositionMatrix, i * Config.AREA_HEIGHT, j * Config.AREA_WIDTH);
                // Find all the different texture ids in the area
                MapAreaAssetData assetData = {
                    vertices,
                    faces,
                    {},
                    blendMap,
                };
                std::set<int> foundTextures = {};
                for (int x = i * Config.AREA_HEIGHT; x < i * Config.AREA_HEIGHT + Config.AREA_HEIGHT; x++)
                {
                    for (int y = j * Config.AREA_WIDTH; y < j * Config.AREA_WIDTH + Config.AREA_WIDTH; y++)
                    {
                        if (assetData.textures.size() < Config.MAX_MAP_TEXTURES)
                        {
                            const int& textureId = texturePositionMatrix[x][y];
                            // Check if texture is already in the list
                            if (foundTextures.find(textureId) == foundTextures.end())
                            {
                                assetData.textures.push_back(textureDataMap[textureId]);
                                foundTextures.insert(textureId);
                            }
                        }
                    }
                }

                mapAreaAssetDataMap[key] = assetData;
            }
            else
            {
                std::cout << "Failed to load area assets: " << areaName << std::endl;
            }
        }
    }

    return mapAreaAssetDataMap;
}

/**
 * @brief Loads the item assets data
 * @param gameItems GameItems
 * @return Map of item id and its corresponding asset data (ItemAssetData)
 */
std::map<int, ItemAssetData> AssetCache::loadItemAssetData(const GameItems& gameItems)
{
    std::map<std::string, ImageData> itemTextures = loadItemTextures(gameItems);

    std::map<int, ItemAssetData> itemAssetDataMap;

    // TODO: All the items can be looped in a single for loop
    for (const auto& generalItem : gameItems.generalItems)
    {
        const std::string objFilepath = Config.OBJECT_OBJ_PATH + generalItem.second.objFilename;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> faces;

        bool success = loadOBJFile(objFilepath, vertices, faces);
        if (success)
        {
            itemAssetDataMap[generalItem.first] = {
                vertices,
                faces,
                itemTextures[generalItem.second.textureFilename],
                itemTextures[generalItem.second.iconFilename]};
        }
    }

    for (const auto& foodItem : gameItems.foodItems)
    {
        const std::string objFilepath = Config.OBJECT_OBJ_PATH + foodItem.second.objFilename;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> faces;

        bool success = loadOBJFile(objFilepath, vertices, faces);
        if (success)
        {
            itemAssetDataMap[foodItem.first] = {
                vertices,
                faces,
                itemTextures[foodItem.second.textureFilename],
                itemTextures[foodItem.second.iconFilename]};
        }
    }

    for (const auto& equippableItem : gameItems.equippableItems)
    {
        const std::string objFilepath = Config.OBJECT_OBJ_PATH + equippableItem.second.objFilename;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> faces;

        bool success = loadOBJFile(objFilepath, vertices, faces);
        if (success)
        {
            itemAssetDataMap[equippableItem.first] = {
                vertices,
                faces,
                itemTextures[equippableItem.second.textureFilename],
                itemTextures[equippableItem.second.iconFilename]};
        }
    }

    return itemAssetDataMap;
}

ImageData AssetCache::getDefaultTexture()
{
    return defaultTextureM;
}
