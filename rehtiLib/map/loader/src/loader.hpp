#pragma once

#include <exception>
#include <filesystem>

#include "utils.hpp"

const unsigned AREA_WIDTH = 128;
const unsigned AREA_HEIGHT = AREA_WIDTH;

// Paths
const std::string ROOT_PATH = "../../../";
const std::string ASSET_PATH = ROOT_PATH + "assets/map/";
const std::string AREA_FILES_PATH = ASSET_PATH + "areas/";
const std::string AREA_MAP_PATH = ASSET_PATH + "map.json";
const std::string OBJECT_TILE_MAPS_PATH = ASSET_PATH + "object_tile_maps/";
const std::string GENERATED_ASSETS_PATH = ROOT_PATH + "assets/generated/";

// Object tile map symbols
const std::string OBJECT_TILE_MAP_CENTER = "X";
const std::string OBJECT_TILE_MAP_NORTH_BLOCK = "N";
const std::string OBJECT_TILE_MAP_EAST_BLOCK = "E";
const std::string OBJECT_TILE_MAP_SOUTH_BLOCK = "S";
const std::string OBJECT_TILE_MAP_WEST_BLOCK = "W";
const std::string OBJECT_TILE_MAP_FULL_BLOCK = "B";
const std::string OBJECT_TILE_MAP_NO_BLOCK = " ";

const unsigned NON_OBJECT_ID = 255 * 255;

// Fetches the area map from the JSON file. Throws an exception if the file corrupted.
static const std::vector<std::vector<std::string>> fetchAreaMap()
{
  rapidjson::Document doc = readJson(AREA_MAP_PATH);

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

/*
 * Create a height map from the area map. The height map tells the height of each tile.
 * It is formed by:
 *
 * 1. Going through the area map and reading the corresponding image file for each area.
 *
 * 2. Going through the image file and reading the height of each pixel. Pixels height is calculated by G * B. But the Green values first bit is a sign bit.
 */
static const std::vector<std::vector<int>> createHeightMap(const std::vector<std::vector<std::string>> &areaMap)
{
  std::vector<std::vector<int>> heightMap;

  // Populate the height map with 0s.
  populateMatrix(heightMap, areaMap, 0, AREA_WIDTH, AREA_HEIGHT);

  // Loop through the area map
  for (unsigned currentAreaRowIndex = 0; currentAreaRowIndex < areaMap.size(); currentAreaRowIndex++)
  {
    std::vector<std::string> areaRow = areaMap[currentAreaRowIndex];

    // For each area read the image file and check the height of each pixel.
    for (unsigned currentAreaColumnIndex = 0; currentAreaColumnIndex < areaRow.size(); currentAreaColumnIndex++)
    {
      const std::string &area = areaRow[currentAreaColumnIndex];
      std::string areaFile = AREA_FILES_PATH + area + ".png";
      std::vector<unsigned char> image; // Represents image pixel map. "RGBARGBARGBA..."
      unsigned width, height;
      readPng(image, width, height, areaFile);
      if (width != AREA_WIDTH || height != AREA_HEIGHT)
      {
        throw std::invalid_argument("Image size is not " + std::to_string(AREA_WIDTH) + "x" + std::to_string(AREA_HEIGHT));
      }

      for (unsigned i = 0; i < height; ++i)
      {
        const unsigned indexY = currentAreaRowIndex * AREA_HEIGHT + i;
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

          const unsigned indexX = currentAreaColumnIndex * AREA_WIDTH + j;
          heightMap[indexY][indexX] = height;
        }
      }
    }
  }

  return heightMap;
}

// Reads object tile map from the JSON file. Throws an exception if the file corrupted or not found.
static const std::vector<std::vector<std::string>> readObjectTileMap(unsigned objectId, const std::vector<std::string> &objectTileMapFilenames)
{
  std::string objectTileMapFile;
  for (const std::string &objectTileMapName : objectTileMapFilenames)
  {
    if (objectTileMapName.find(std::to_string(objectId)) != std::string::npos)
    {
      objectTileMapFile = objectTileMapName;
      break;
    }
  }

  if (objectTileMapFile.empty())
  {
    throw std::domain_error("Object tile map file not found for object id " + std::to_string(objectId));
  }

  // Read the object tile map for the object.
  rapidjson::Document doc = readJson(OBJECT_TILE_MAPS_PATH + objectTileMapFile);

  if (!doc.IsArray())
  {
    throw std::runtime_error("Object tile map JSON is not an array");
  }

  std::vector<std::vector<std::string>> objectTileMap;
  for (rapidjson::SizeType i = 0; i < doc.Size(); ++i)
  {
    if (!doc[i].IsArray())
    {
      throw std::runtime_error("Object tile map inner structure is not an array");
    }

    std::vector<std::string> objectTileMapRow;
    for (rapidjson::SizeType j = 0; j < doc[i].Size(); ++j)
    {
      if (!doc[i][j].IsString())
      {
        throw std::runtime_error("Object tile map element is not a string");
      }

      objectTileMapRow.push_back(doc[i][j].GetString());
    }
    objectTileMap.push_back(objectTileMapRow);
  }

  return objectTileMap;
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
        centerXMap = currentAreaColumnIndex * AREA_WIDTH + areaX + j;
        centerYMap = currentAreaRowIndex * AREA_HEIGHT + areaY + i;
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

/*
 * When object tile map is rotated, it block direction needs to be rotated as well.
 * For example, if the object is rotated 90 degrees clockwise, the south block becomes west block and so on.
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

/*
 * Generate the object block map. The map defines how the objects block the tiles around itself.
 * Also, fills in the given object map with object id's (for optimization).
 *
 * 1. Go through the area map and read the corresponding image file for each area. (<area_name>-obj.png).
 *
 * 2. For each area read the image file and check if there are any objects in the tile. Object is defined if R * G is not 255 * 255.
 *
 * 3. If there is an object in the tile, read the object tile map <id>-<object_name>.json. Check the block map definition from the README.
 *
 * 4. Input the object tile map to the object block map. Consider the rotation of the object as well. Rotation is stored in the B-value.
 */
static const std::vector<std::vector<std::string>> createObjectBlockMap(const std::vector<std::vector<std::string>> &areaMap, std::vector<std::vector<unsigned>> &objectIdMap)
{
  // Populate the object block map with non-blocked tiles
  std::vector<std::vector<std::string>> objectBlockMap;
  populateMatrix(objectBlockMap, areaMap, OBJECT_TILE_MAP_NO_BLOCK, AREA_WIDTH, AREA_HEIGHT);

  // Populate the object map with NON_OBJECT_IDs
  populateMatrix(objectIdMap, areaMap, NON_OBJECT_ID, AREA_WIDTH, AREA_HEIGHT);

  // Read object tile maps
  const std::vector<std::string> objectTileMapNames = readDirectory(OBJECT_TILE_MAPS_PATH, ".json");

  for (unsigned currentAreaRowIndex = 0; currentAreaRowIndex < areaMap.size(); currentAreaRowIndex++)
  {
    const std::vector<std::string> areaRow = areaMap[currentAreaRowIndex];
    for (unsigned currentAreaColumnIndex = 0; currentAreaColumnIndex < areaRow.size(); currentAreaColumnIndex++)
    {
      const std::string &area = areaRow[currentAreaColumnIndex];
      // Read the object tile map for the area.
      std::vector<unsigned char> image; // Represents image pixel map. "RGBARGBARGBA..."
      unsigned width, height;
      std::string filepath = AREA_FILES_PATH + area + "-obj.png";
      readPng(image, width, height, filepath);
      if (width != AREA_WIDTH || height != AREA_HEIGHT)
      {
        throw std::invalid_argument("Image size is not " + std::to_string(AREA_WIDTH) + "x" + std::to_string(AREA_HEIGHT));
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
            int rotation = b > 3 ? 0 : b; // Rotation is stored in the B-value. If B is 4 or more, the object the object is not rotated from its original position.

            std::vector<std::vector<std::string>> objectTileMap = readObjectTileMap(objectId, objectTileMapNames);

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

            insertObjectTileMap(objectBlockMap, objectTileMap, i, j, currentAreaRowIndex, currentAreaColumnIndex);
            objectIdMap[currentAreaRowIndex * AREA_HEIGHT + i][currentAreaColumnIndex * AREA_WIDTH + j] = objectId;
            std::cout << "Object id: " << objectId << " inserted " << std::endl;
          }
        }
      }
    }
  }

  return objectBlockMap;
}

/*
 * Generates access map from height map & object block map. Access map defines which tiles are accessible from which tiles.
 *
 * Each cell contains 4-bit value. 1st bit is the rightmost bit
 * 1. bit = north
 * 2. bit = east
 * 3. bit = south
 * 4. bit = west.
 *
 * If bit = 1, the tile is accessible from that direction. Otherwise not.
 *
 * North is blocked if, the tile is on the top row, height difference with the north tile is > 1 or the tile has a north block (N).
 * East is blocked if, the tile is on the rightmost column, height difference with the east tile is > 1 or the tile has a east block (E).
 * South is blocked if, the tile is on the bottom row, height difference with the south tile is > 1 or the tile has a south block (S).
 * West is blocked if, the tile is on the leftmost column, height difference with the west tile is > 1 or the tile has a west block (W).
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
      if (i == 0 || height - heightMap[i - 1][j] > 1 || objectBlockMap[i][j].find("N") != std::string::npos)
      {
        // Block the north bit
        access = access & 0b1110;
      }

      // Check East
      if (j == heightMap[i].size() - 1 || height - heightMap[i][j + 1] > 1 || objectBlockMap[i][j].find("E") != std::string::npos)
      {
        // Block the east bit
        access = access & 0b1101;
      }

      // Check South
      if (i == heightMap.size() - 1 || height - heightMap[i + 1][j] > 1 || objectBlockMap[i][j].find("S") != std::string::npos)
      {
        // Block the south bit
        access = access & 0b1011;
      }

      // Check West
      if (j == 0 || height - heightMap[i][j - 1] > 1 || objectBlockMap[i][j].find("W") != std::string::npos)
      {
        // Block the west bit
        access = access & 0b0111;
      }
    }
  }

  return accessMap;
}