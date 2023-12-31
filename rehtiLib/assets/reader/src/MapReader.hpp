#pragma once

/**
 * @file The map reader is responsibe for reading the generated map assets on runtime. Map assets are generated by the loader.
 */

#include <vector>
#include <string>

/**
 * @brief Loads the access map from the given path.
 * @return Matrix of uint8_t that contains the access map information.
 */
std::vector<std::vector<uint8_t>> fetchAccessMatrix();

/**
 * @brief Loads the height map from the given path.
 * @return Matrix of int that will be filled with the height map information. Each cell contains the height of the tile.
 */
std::vector<std::vector<int>> fetchHeightMatrix();

/**
 * @brief Loads the map texture map from the given path.
 * @return Matrix of int that will be filled with the map texture map information.
 */
std::vector<std::vector<int>> fetchMapTextureMatrix();

/**
 * @brief Loads the area map from the given path.
 * @return Matrix of string that will be filled with the area map information.
 */
std::vector<std::vector<std::string>> fetchAreaMatrix();