#pragma once

#include <string>

/**
 * @file Contains the configuration values for the game
 */

const std::string ROOT_PATH = "./";

/**
 * @brief Contains the configuration values for the game
 */
static struct
{
    const unsigned AREA_WIDTH = 64;          ///< Width of the area in tiles
    const unsigned AREA_HEIGHT = AREA_WIDTH; ///< The height is the same as the width

    // Paths
    const std::string ASSET_PATH = ROOT_PATH + "assets/map/";
    const std::string GENERATED_ASSETS_PATH = ROOT_PATH + "assets/generated/";
    const std::string AREA_FILES_PATH = ASSET_PATH + "areas/";
    const std::string AREA_MAP_PATH = ASSET_PATH + "map.json";
    const std::string CHARACTER_JSON_PATH = ROOT_PATH + "assets/characters.json";
    const std::string ITEM_JSON_PATH = "./assets/items.json";
    const std::string OBJECT_JSON_PATH = ROOT_PATH + "assets/objects.json";
    const std::string SKILL_JSON_PATH = ROOT_PATH + "assets/skills.json";
    const std::string OBJECT_OBJ_PATH = ROOT_PATH + "assets/objects/";
    const std::string TEXTURE_PATH = ROOT_PATH + "assets/textures/";
    const std::string MAP_TEXTURE_MAP_PATH = TEXTURE_PATH + "map_textures.json";
    const std::string DEFAULT_TEXTURE_PATH = TEXTURE_PATH + "defaultTexture.jpg";
    const std::string ANIMATION_JSON_PATH = ROOT_PATH + "assets/animations.json";
    const std::string ITEM_ICON_PATH = ROOT_PATH + "assets/item/";

    // Paths to generated files
    const std::string GENERATED_AREA_MAP_PATH = GENERATED_ASSETS_PATH + "area_map.txt";
    const std::string GENERATED_ACCESS_MAP_PATH = GENERATED_ASSETS_PATH + "access_map.txt";
    const std::string GENERATED_HEIGHT_MAP_PATH = GENERATED_ASSETS_PATH + "height_map.txt";
    const std::string GENERATED_OBJECT_JSON_PATH = GENERATED_ASSETS_PATH + "objects.json";
    const std::string GENERATED_AREA_OBJ_PATH = GENERATED_ASSETS_PATH + "areas/";
    const std::string GENERATED_MAP_TEXTURE_MAP_PATH = GENERATED_ASSETS_PATH + "map_texture_map.txt";

    const float HEIGHT_MAP_SCALE = 0.15;        ///< Percentage value of how much the height is scaled from the original unit.
    const float TILE_SIDE_SCALE = 1.0;          ///< Percentage value of how much the side is scaled from the original unit.
    const float TILE_SIDE_UNIT = 1.0;           ///< The size of the side of a tile in the original unit.
    const unsigned int MAX_MAP_TEXTURES = 5;    ///< Maximum amount of different textures in the map.
    const unsigned int MAX_PASSABLE_HEIGHT = 5; ///< Maximum height difference between two tiles that is still passable by entities.
} Config;
