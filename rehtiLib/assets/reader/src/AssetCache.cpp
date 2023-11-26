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
    characterAssetDataM = loadCharacterAssetData(gameCharacters);
    areaAssetDataM = loadAreaAssetData(areaMatrix);
    // TODO: Load item asset data
    std::cout << "ASSET CACHE - Assets loaded" << std::endl;
}

std::map<std::string, MapAreaAssetData>& AssetCache::getAreaAssetData()
{
    return areaAssetDataM;
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
        std::vector<aiVector3D> vertices;
        std::vector<aiFace> faces;

        bool success = loadOBJFile(objFilepath, vertices, faces);
        if (success)
        {
            objectAssetDataMap[object.id] = {aiVector3DVectorToVertexVector(vertices), aiFaceVectorToFaceVector(faces), textureDataMap[object.textureFilename], object.characterInteractAnimation};
        }
    }

    return objectAssetDataMap;
}

std::map<int, CharacterAssetData> AssetCache::loadCharacterAssetData(const GameCharacters& gameCharacters)
{
    // Load character textures first
    std::map<std::string, ImageData> textureDataMap = loadCharacterTextures(gameCharacters);

    std::map<int, CharacterAssetData> characterAssetDataMap;

    return characterAssetDataMap;
}

std::map<std::string, MapAreaAssetData> AssetCache::loadAreaAssetData(const std::vector<std::vector<std::string>>& areaMap)
{
    // Load map textures first
    std::map<int, ImageData> textureDataMap = loadMapTextures();
    std::vector<std::vector<int>> texturePositionMatrix = loadMapTexturePositions();

    std::map<std::string, MapAreaAssetData> mapAreaAssetDataMap;

    for (int i = 0; i < areaMap.size(); i++)
    {
        for (int j = 0; j < areaMap[i].size(); j++)
        {
            std::vector<aiVector3D> vertices;
            std::vector<aiFace> faces;
            const std::string areaName = areaMap[i][j];
            const std::string key = areaName + std::to_string(i) + std::to_string(j);

            bool success = loadOBJFile(Config.GENERATED_AREA_OBJ_PATH + key + ".obj", vertices, faces);
            if (success)
            {
                const ImageData blendMap = createAreaBlendMap(texturePositionMatrix, i * Config.AREA_WIDTH, j * Config.AREA_HEIGHT);
                // Find all the different texture ids in the area
                MapAreaAssetData assetData = {
                    aiVector3DVectorToVertexVector(vertices),
                    aiFaceVectorToFaceVector(faces),
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

ImageData AssetCache::getDefaultTexture()
{
    return defaultTextureM;
}
