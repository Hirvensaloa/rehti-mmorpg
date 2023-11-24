#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <exception>
#include <filesystem>

#include "RehtiReader.hpp"
#include "RehtiUtils.hpp"
#include "Utils.hpp"

// Object tile map symbols
const std::string OBJECT_TILE_MAP_CENTER = "X";
const std::string OBJECT_TILE_MAP_NORTH_BLOCK = "N";
const std::string OBJECT_TILE_MAP_EAST_BLOCK = "E";
const std::string OBJECT_TILE_MAP_SOUTH_BLOCK = "S";
const std::string OBJECT_TILE_MAP_WEST_BLOCK = "W";
const std::string OBJECT_TILE_MAP_FULL_BLOCK = "B";
const std::string OBJECT_TILE_MAP_NO_BLOCK = " ";

const unsigned NON_OBJECT_ID = 255 * 255;

/**
 * @file Contains functions for loading the map assets from images and other definitions.
 */

// Fetches the area map from the JSON file. Throws an exception if the file corrupted.
static const std::vector<std::vector<std::string>> fetchAreaMap()
{
  rapidjson::Document doc = readJson(Config.AREA_MAP_PATH);

  if (!doc.IsArray())
  {
    throw std::runtime_error("JSON is not an array");
  }

  std::vector<std::vector<std::string>> areaMap;

  for (rapidjson::SizeType i = 0; i < doc.Size(); ++i)
  {
    if (!doc[i].IsArray())
    {
      throw std::runtime_error("Inner structure is not an array");
    }

    std::vector<std::string> areaRow;
    for (rapidjson::SizeType j = 0; j < doc[i].Size(); ++j)
    {
      if (!doc[i][j].IsString())
      {
        throw std::runtime_error("Element is not a string");
      }

      areaRow.push_back(doc[i][j].GetString());
    }
    areaMap.push_back(areaRow);
  }

  return areaMap;
}

/**
 * @brief Loads a height map and map's texture map into given parameters. Loads them together to avoid reading the same image file twice.
 *
 * Height map is formed by:
 * @brief 1. Going through the area map and reading the corresponding image file for each area.
 * @brief 2. Going through the image file and reading the height of each pixel. Pixels height is calculated by G * B. But the Green values first bit is a sign bit.
 *
 * Map's texture map is formed by:
 * @brief 1. Going through the area map and reading the corresponding image file for each area.
 * @brief 2. Going through the image file and reading the texture id of each pixel. Texture id is the R value.
 *
 * @param areaMap Matrix of all the area names
 * @param heightMap Empty height matrix to be filled
 * @param textureMap Empty texture map to be filled
 */
static void loadHeightAndTextureMap(const std::vector<std::vector<std::string>> &areaMap, std::vector<std::vector<int>> &heightMap, std::vector<std::vector<int>> &textureMap)
{
  // Populate the height map with 0s.
  populateMatrix(heightMap, areaMap, 0, Config.AREA_WIDTH, Config.AREA_HEIGHT);
  populateMatrix(textureMap, areaMap, 0, Config.AREA_WIDTH, Config.AREA_HEIGHT);

  // Loop through the area map
  for (unsigned currentAreaRowIndex = 0; currentAreaRowIndex < areaMap.size(); currentAreaRowIndex++)
  {
    std::vector<std::string> areaRow = areaMap[currentAreaRowIndex];

    // For each area read the image file and check the height of each pixel.
    for (unsigned currentAreaColumnIndex = 0; currentAreaColumnIndex < areaRow.size(); currentAreaColumnIndex++)
    {
      const std::string &area = areaRow[currentAreaColumnIndex];
      std::string areaFile = Config.AREA_FILES_PATH + area + ".png";
      std::vector<unsigned char> image; // Represents image pixel map. "RGBARGBARGBA..."
      unsigned width, height;
      readPng(image, width, height, areaFile);
      if (width != Config.AREA_WIDTH || height != Config.AREA_HEIGHT)
      {
        throw std::invalid_argument("Image size is not " + std::to_string(Config.AREA_WIDTH) + "x" + std::to_string(Config.AREA_HEIGHT));
      }

      for (unsigned i = 0; i < height; ++i)
      {
        const unsigned indexY = currentAreaRowIndex * Config.AREA_HEIGHT + i;
        for (unsigned j = 0; j < width; ++j)
        {
          unsigned pixelIndex = (i * width + j) * 4;
          unsigned char r = image[pixelIndex + 0];
          unsigned char g = image[pixelIndex + 1];
          unsigned char b = image[pixelIndex + 2];

          // The height is calculated by G * B. But the Green values first bit is a sign bit.
          int height = (g & 0x7F) * b;
          if (g & 0x80)
          {
            height *= -1;
          }

          const unsigned indexX = currentAreaColumnIndex * Config.AREA_WIDTH + j;
          textureMap[indexY][indexX] = r;
          heightMap[indexY][indexX] = height;
        }
      }
    }
  }
}

// Inserts the object tile map to the map.
static void insertObjectTileMap(std::vector<std::vector<std::string>> &objectBlockMap, std::vector<std::vector<std::string>> &objectTileMap, unsigned areaY, unsigned areaX, unsigned currentAreaRowIndex, unsigned currentAreaColumnIndex)
{
  // First find the object's center tile
  unsigned centerX, centerY;
  unsigned centerXMap, centerYMap;
  bool shouldBreak = false;
  for (unsigned i = 0; i < objectTileMap.size(); i++)
  {
    for (unsigned j = 0; j < objectTileMap[i].size(); j++)
    {
      if (objectTileMap[i][j].find(OBJECT_TILE_MAP_CENTER) != std::string::npos)
      {
        // Set the center tile's position in the object tile map
        centerX = j;
        centerY = i;
        // Calculate the center tile's position in the object block map
        centerXMap = currentAreaColumnIndex * Config.AREA_WIDTH + areaX + j;
        centerYMap = currentAreaRowIndex * Config.AREA_HEIGHT + areaY + i;
        shouldBreak = true;
        break;
      }
    }

    if (shouldBreak)
    {
      break;
    }
  }

  // Insert the object tile map to the object block map
  for (unsigned i = 0; i < objectTileMap.size(); i++)
  {
    for (unsigned j = 0; j < objectTileMap[i].size(); j++)
    {
      const int xDiff = j - centerX;
      const int yDiff = i - centerY;
      const int x = centerXMap + xDiff;
      const int y = centerYMap + yDiff;

      // Check if object is out of bounds (or it's tiles are out of bounds )
      if (x < 0 || x >= objectBlockMap[0].size() || y < 0 || y >= objectBlockMap.size())
      {
        throw std::out_of_range("Object out of bounds");
      }

      objectBlockMap[y][x] = objectTileMap[i][j];
    }
  }
}

/**
 * @brief When object tile map is rotated, its direction needs to be rotated as well.
 *
 * For example, if the object is rotated 90 degrees clockwise, the south block becomes west block and so on.
 *
 * @param objectTileMap The object tile map to be rotated.
 * @param rotation The rotation of the object. 0-4 (North, East, South, West)
 *
 * @return void, but modifies the objectTileMap.
 */
static void changeBlockDirection(std::vector<std::vector<std::string>> &objectTileMap, unsigned rotation)
{
  const std::vector<std::string> symbols{OBJECT_TILE_MAP_NORTH_BLOCK, OBJECT_TILE_MAP_EAST_BLOCK, OBJECT_TILE_MAP_SOUTH_BLOCK, OBJECT_TILE_MAP_WEST_BLOCK};

  for (unsigned i = 0; i < objectTileMap.size(); i++)
  {
    for (unsigned j = 0; j < objectTileMap[i].size(); j++)
    {
      for (unsigned k = 0; k < symbols.size(); k++)
      {
        const auto index = objectTileMap[i][j].find(symbols[k]);
        if (index != std::string::npos)
        {
          const unsigned newSymbolIndex = (k + rotation) % symbols.size();
          objectTileMap[i][j].replace(index, symbols[k].size(), symbols[newSymbolIndex]);
          break;
        }
      }
    }
  }
}

/**
 * @brief Generate the object block map. The map defines how the objects block the tiles around itself.
 *
 * 1. Go through the area map and read the corresponding image file for each area. (<area_name>-obj.png).
 *
 * 2. For each area read the image file and check if there are any objects in the tile. Object is defined if R * G is not 255 * 255.
 *
 * 3. If there is an object in the tile, read the object tile map provided by the gameObjects parameter.
 *
 * 4. Input the object tile map to the object block map. Consider the rotation of the object as well. Rotation is stored in the B-value.
 *
 * ALSO, stores objects with their locations to generated/objects.json. This is used by the game server to spawn the objects on start.
 *
 * @param areaMap Matrix of all the area names.
 * @param gameObjects A GameObjects object containing all the object definitions.
 * @param heightMap A matrix representing the height of the map.
 * @returns const std::vector<std::vector<std::string>>
 */
static const std::vector<std::vector<std::string>> createObjectBlockMap(const std::vector<std::vector<std::string>> &areaMap, GameObjects gameObjects, const std::vector<std::vector<int>> &heightMap)
{
  // Populate the object block map with non-blocked tiles
  std::vector<std::vector<std::string>> objectBlockMap;
  populateMatrix(objectBlockMap, areaMap, OBJECT_TILE_MAP_NO_BLOCK, Config.AREA_WIDTH, Config.AREA_HEIGHT);

  std::vector<ObjectLocation> objectsLocations;

  for (unsigned currentAreaRowIndex = 0; currentAreaRowIndex < areaMap.size(); currentAreaRowIndex++)
  {
    const std::vector<std::string> areaRow = areaMap[currentAreaRowIndex];
    for (unsigned currentAreaColumnIndex = 0; currentAreaColumnIndex < areaRow.size(); currentAreaColumnIndex++)
    {
      const std::string &area = areaRow[currentAreaColumnIndex];
      // Read the object tile map for the area.
      std::vector<unsigned char> image; // Represents image pixel map. "RGBARGBARGBA..."
      unsigned width, height;
      std::string filepath = Config.AREA_FILES_PATH + area + "-obj.png";
      readPng(image, width, height, filepath);
      if (width != Config.AREA_WIDTH || height != Config.AREA_HEIGHT)
      {
        throw std::invalid_argument("Image size is not " + std::to_string(Config.AREA_WIDTH) + "x" + std::to_string(Config.AREA_HEIGHT));
      }

      // Loop through the image and find all objects
      for (unsigned i = 0; i < height; ++i)
      {
        for (unsigned j = 0; j < width; ++j)
        {
          unsigned pixelIndex = (i * width + j) * 4;
          unsigned r = image[pixelIndex + 0];
          unsigned g = image[pixelIndex + 1];
          unsigned b = image[pixelIndex + 2];

          unsigned objectId = r * g;

          if (objectId == NON_OBJECT_ID)
          {
            continue;
          }

          if (objectId)
          {
            unsigned rotation = b > 3 ? 0 : b; // Rotation is stored in the B-value. If B is 4 or more, the object is not rotated from its original position.

            std::vector<std::vector<std::string>> objectTileMap = gameObjects.getTileMap(objectId);

            if (objectTileMap.empty())
            {
              throw std::runtime_error("Object tile map not found for id: " + std::to_string(objectId));
            }

            std::cout << "Object id: " << objectId << " read " << std::endl;

            if (rotation)
            {
              // Rotate the object tile map
              rotateMatrix(objectTileMap, rotation);
              changeBlockDirection(objectTileMap, rotation);
              std::cout << "Object id: " << objectId << " rotated " << std::endl;
            }
            else
            {
              std::cout << "Object id: " << objectId << " not rotated " << std::endl;
            }

            int x = currentAreaColumnIndex * Config.AREA_WIDTH + j;
            int y = currentAreaRowIndex * Config.AREA_HEIGHT + i;
            int z = heightMap[y][x];
            ObjectLocation objLoc = {objectId, "0", x, y, z, rotation};
            objLoc.instanceId = generateObjectInstanceId(objLoc);
            objectsLocations.push_back(objLoc);

            insertObjectTileMap(objectBlockMap, objectTileMap, i, j, currentAreaRowIndex, currentAreaColumnIndex);
            std::cout << "Object id: " << objectId << " inserted " << std::endl;
          }
        }
      }
    }
  }

  // Write the object locations to a file
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Value objects(rapidjson::kArrayType);
  for (const auto &objLoc : objectsLocations)
  {
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("id", objLoc.id, doc.GetAllocator());
    object.AddMember("x", objLoc.x, doc.GetAllocator());
    object.AddMember("y", objLoc.y, doc.GetAllocator());
    object.AddMember("z", objLoc.z, doc.GetAllocator());
    object.AddMember("rotation", objLoc.rotation, doc.GetAllocator());
    objects.PushBack(object, doc.GetAllocator());
  }
  doc.AddMember("objects", objects, doc.GetAllocator());

  const std::string str = createString(doc);

  std::ofstream objectsFile(Config.GENERATED_OBJECT_JSON_PATH);
  objectsFile << str;
  objectsFile.close();

  return objectBlockMap;
}

/**
 * @brief Generates access map from height map & object block map. Access map defines how tiles can or cannot be accessed from it's neighbours.
 *
 * Access map is defined as a matrix where each cell contains 4-bit value. 1st bit is the rightmost bit
 * 1. bit = north
 * 2. bit = east
 * 3. bit = south
 * 4. bit = west.
 *
 * If bit = 1, the tile is accessible from that direction. Otherwise not.
 *
 * Access map is constructed from the object block map and the height map as follows:
 *
 * North is blocked if, the tile is on the top row, height difference with the north tile is > 1 or the tile has a north block (N).
 * East is blocked if, the tile is on the rightmost column, height difference with the east tile is > 1 or the tile has a east block (E).
 * South is blocked if, the tile is on the bottom row, height difference with the south tile is > 1 or the tile has a south block (S).
 * West is blocked if, the tile is on the leftmost column, height difference with the west tile is > 1 or the tile has a west block (W).
 *
 * @param heightMap A matrix representing the height map.
 * @param objectBlockMap A matrix representing the object block map.
 * @returns A matrix representing the access map.
 */
static const std::vector<std::vector<unsigned>> generateAccessMap(const std::vector<std::vector<int>> &heightMap, const std::vector<std::vector<std::string>> &objectBlockMap)
{
  std::vector<std::vector<unsigned>> accessMap;

  // Populate the access map with 1111s, meaning tiles are accessible from all directions.
  const unsigned defaultValue = 0b1111;
  populateMatrixFromReference(accessMap, heightMap, defaultValue);

  for (int i = 0; i < heightMap.size(); i++)
  {
    for (int j = 0; j < heightMap[i].size(); j++)
    {
      int height = heightMap[i][j];
      unsigned &access = accessMap[i][j];

      // Check North
      if (i <= 0 || heightMap[i - 1].size() <= j || height - heightMap[i - 1][j] > Config.MAX_PASSABLE_HEIGHT || objectBlockMap[i][j].find("N") != std::string::npos)
      {
        // Block the north bit
        access = access & 0b1110;
      }

      // Check East
      if (heightMap[i].size() <= j + 1 || height - heightMap[i][j + 1] > Config.MAX_PASSABLE_HEIGHT || objectBlockMap[i][j].find("E") != std::string::npos)
      {
        // Block the east bit
        access = access & 0b1101;
      }

      // Check South
      if (heightMap.size() <= i + 1 || heightMap[i + 1].size() <= j || height - heightMap[i + 1][j] > Config.MAX_PASSABLE_HEIGHT || objectBlockMap[i][j].find("S") != std::string::npos)
      {
        // Block the south bit
        access = access & 0b1011;
      }

      // Check West
      if (j <= 0 || height - heightMap[i][j - 1] > Config.MAX_PASSABLE_HEIGHT || objectBlockMap[i][j].find("W") != std::string::npos)
      {
        // Block the west bit
        access = access & 0b0111;
      }
    }
  }

  return accessMap;
}
