#include "rapidjson/document.h"
#include <iostream>
#include <string>
#include <vector>

#include "../../Config.hpp"
#include "RehtiUtils.hpp"
#include "TextureReader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

std::map<std::string, ImageData> loadObjectTextures(const GameObjects& gameObjects)
{
    std::map<std::string, ImageData> textures;

    int width, height, channels;
    unsigned char* data;

    std::vector<std::string> textureFilenames;

    for (const auto& object : gameObjects.generalObjects)
    {
        textureFilenames.push_back(object.second.textureFilename);
    }
    for (const auto& object : gameObjects.resourceObjects)
    {
        textureFilenames.push_back(object.second.textureFilename);
    }
    for (const auto& object : gameObjects.lootObjects)
    {
        textureFilenames.push_back(object.second.textureFilename);
    }

    stbi_set_flip_vertically_on_load(true);
    // Read all the textures
    for (const auto& filename : textureFilenames)
    {
        // Skip if the texture is already loaded. Multiple objects can have the same texture
        if (textures.contains(filename))
        {
            continue;
        }
        data = stbi_load((Config.TEXTURE_PATH + filename).c_str(), &width, &height, &channels, 4);
        if (data)
        {
            textures[filename] = ImageData{data, width, height};
            std::cout << "Loaded texture: " << filename << std::endl;
        }
        else
        {
            throw std::runtime_error("Failed to load texture: " + filename);
        }
    }

    stbi_set_flip_vertically_on_load(false);
    return textures;
}

std::map<std::string, ImageData> loadCharacterTextures(const GameCharacters& gameCharacters)
{
    std::vector<std::string> textureFilenames;

    textureFilenames.push_back(gameCharacters.player.textureFilename);
    for (const auto& npc : gameCharacters.npcs)
    {
        textureFilenames.push_back(npc.textureFilename);
    }

    std::map<std::string, ImageData> textures;

    int width, height, channels;
    unsigned char* data;

    for (const auto& filename : textureFilenames)
    {
        // Skip if the texture is already loaded. Multiple characters can have the same texture
        if (textures.contains(filename))
        {
            continue;
        }
        stbi_set_flip_vertically_on_load(false);
        data = stbi_load((Config.TEXTURE_PATH + filename).c_str(), &width, &height, &channels, 4);
        if (data)
        {
            textures[filename] = ImageData{data, width, height};
            std::cout << "Loaded texture: " << filename << std::endl;
        }
        else
        {
            throw std::runtime_error("Failed to load character texture: " + filename);
        }
    }

    return textures;
}

std::map<std::string, ImageData> loadItemTextures(const GameItems& gameItems)
{
    std::map<std::string, ImageData> textures;

    int width, height, channels;
    unsigned char* data;

    std::vector<std::string> textureFilenames;
    std::vector<std::string> iconFilenames;

    // TODO: Items can be looped in a single loop
    for (const auto& item : gameItems.generalItems)
    {
        textureFilenames.push_back(item.second.textureFilename);
        iconFilenames.push_back(item.second.iconFilename);
    }

    for (const auto& item : gameItems.equippableItems)
    {
        textureFilenames.push_back(item.second.textureFilename);
        iconFilenames.push_back(item.second.iconFilename);
    }

    for (const auto& item : gameItems.foodItems)
    {
        textureFilenames.push_back(item.second.textureFilename);
        iconFilenames.push_back(item.second.iconFilename);
    }

    // TODO: This can also be done in a single loop
    // Load all the obj textures
    for (const auto& filename : textureFilenames)
    {
        if (textures.contains(filename))
        {
            continue;
        }
        data = stbi_load((Config.TEXTURE_PATH + filename).c_str(), &width, &height, &channels, 4);
        if (data)
        {
            textures[filename] = ImageData{data, width, height};
            std::cout << "Loaded texture: " << filename << std::endl;
        }
        else
        {
            throw std::runtime_error("Failed to load texture: " + filename);
        }
    }

    // Load all the icon textures
    for (const auto& filename : iconFilenames)
    {
        if (textures.contains(filename))
        {
            continue;
        }
        stbi_set_flip_vertically_on_load(false);
        data = stbi_load((Config.ITEM_ICON_PATH + filename).c_str(), &width, &height, &channels, 4);
        if (data)
        {
            textures[filename] = ImageData{data, width, height};
            std::cout << "Loaded icon texture: " << filename << std::endl;
        }
        else
        {
            throw std::runtime_error("Failed to load icon texture: " + filename);
        }
    }

    return textures;
}

std::map<int, ImageData> loadMapTextures()
{
    rapidjson::Document document = readJson(Config.MAP_TEXTURE_MAP_PATH);

    // Ensure the root is an object
    if (!document.IsArray())
    {
        throw std::runtime_error("Map textures JSON file is not an array.");
    }

    std::map<int, ImageData> textureMap;

    rapidjson::Value& textureArray = document.GetArray();

    int width, height, channels;
    unsigned char* data;

    for (rapidjson::SizeType i = 0; i < textureArray.Size(); i++)
    {
        const rapidjson::Value& texture = textureArray[i];

        if (!validMember(texture, "id", ValueType::INT) || !validMember(texture, "filename", ValueType::STRING))
        {
            throw std::runtime_error("JSON Validation failed for texture index: " + std::to_string(i));
        }

        int id = texture["id"].GetInt();
        std::string filename = texture["filename"].GetString();

        data = stbi_load((Config.TEXTURE_PATH + filename).c_str(), &width, &height, &channels, 4);
        if (data)
        {
            textureMap[id] = ImageData{data, width, height};
            std::cout << "Loaded texture: " << filename << std::endl;
        }
        else
        {
            throw std::runtime_error("Failed to map load texture: " + filename);
        }
    }

    return textureMap;
}

std::vector<std::vector<int>> loadMapTexturePositions()
{
    std::vector<std::vector<std::string>> mapTexturePositionMatrixStr;
    readMatrixFromFile(mapTexturePositionMatrixStr, Config.GENERATED_MAP_TEXTURE_MAP_PATH);
    std::vector<std::vector<int>> mapTexturePositionMatrix;
    for (const auto& row : mapTexturePositionMatrixStr)
    {
        std::vector<int> rowInt;
        for (const auto& cell : row)
        {
            rowInt.push_back(std::stoi(cell));
        }
        mapTexturePositionMatrix.push_back(rowInt);
    }

    return mapTexturePositionMatrix;
}

ImageData createAreaBlendMap(const std::vector<std::vector<int>>& mapTexturePositionMatrix, int columnOffset, int rowOffset)
{
    std::vector<int> textureIds;
    for (int i = rowOffset; i < rowOffset + Config.AREA_WIDTH; i++)
    {
        for (int j = columnOffset; j < columnOffset + Config.AREA_HEIGHT; j++)
        {
            const int textureId = mapTexturePositionMatrix[i][j];
            if (std::find(textureIds.begin(), textureIds.end(), textureId) == textureIds.end() && textureIds.size() < Config.MAX_MAP_TEXTURES)
            {
                textureIds.push_back(textureId);
            }

            // Find the texture index
            int textureIndex = -1;
            for (int k = 0; k < textureIds.size(); k++)
            {
                if (textureIds[k] == textureId)
                {
                    textureIndex = k;
                    break;
                }
            }

            // if it is not found, throw an error because we have too many textures
            if (textureIndex == -1)
            {
                throw std::runtime_error("Too many textures in one area. Maximum is " + std::to_string(Config.MAX_MAP_TEXTURES));
            }
        }
    }

    // Create the blend map
    int width = Config.AREA_WIDTH;
    int height = Config.AREA_HEIGHT;
    unsigned char* data = new unsigned char[width * height * 4];

    for (int j = rowOffset; j < rowOffset + Config.AREA_WIDTH; j++)
    {
        for (int k = columnOffset; k < columnOffset + Config.AREA_HEIGHT; k++)
        {
            int blendMapIndex = j * width * 4 + k * 4;

            data[blendMapIndex] = textureIds.size() > 1 && mapTexturePositionMatrix[j][k] == textureIds[1] ? 255 : 0;
            data[blendMapIndex + 1] = textureIds.size() > 2 && mapTexturePositionMatrix[j][k] == textureIds[2] ? 255 : 0;
            data[blendMapIndex + 2] = textureIds.size() > 3 && mapTexturePositionMatrix[j][k] == textureIds[3] ? 255 : 0;
            data[blendMapIndex + 3] = textureIds.size() > 4 && mapTexturePositionMatrix[j][k] == textureIds[4] ? 255 : 0;
        }
    }

    return ImageData{data, width, height};
}

ImageData loadDefaultTexture()
{
    int width, height, channels;
    unsigned char* data;

    data = stbi_load(Config.DEFAULT_TEXTURE_PATH.c_str(), &width, &height, &channels, 4);
    if (data)
    {
        std::cout << "Loaded default texture: " << Config.DEFAULT_TEXTURE_PATH << std::endl;
        return ImageData{data, width, height};
    }
    else
    {
        throw std::runtime_error("Failed to load default texture: " + Config.DEFAULT_TEXTURE_PATH);
    }
}