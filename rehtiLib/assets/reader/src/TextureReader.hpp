#pragma once

#include <map>
#include <string>

struct StbImageData
{
  unsigned char *data;
  int width;
  int height;
};

/**
 * @file Reads the texture files
 */

/**
 * @brief Reads all the textures in textures folder to a map (key=filename, value=StbImageData)
 * @return Map of texture filename and its corresponding StbImageData
 */
std::map<std::string, StbImageData> loadTextures();
