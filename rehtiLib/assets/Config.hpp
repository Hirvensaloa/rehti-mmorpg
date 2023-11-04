#pragma once

#include <string>

const std::string ROOT_PATH = "./";

struct
{
  const unsigned AREA_WIDTH = 16;
  const unsigned AREA_HEIGHT = AREA_WIDTH;

  // Paths
  const std::string ASSET_PATH = ROOT_PATH + "assets/map/";
  const std::string GENERATED_ASSETS_PATH = ROOT_PATH + "assets/generated/";
  const std::string AREA_FILES_PATH = ASSET_PATH + "areas/";
  const std::string AREA_MAP_PATH = ASSET_PATH + "test.json";
  const std::string ITEM_JSON_PATH = "./assets/items.json";
  const std::string OBJECT_JSON_PATH = ROOT_PATH + "assets/objects.json";
  const std::string SKILL_JSON_PATH = ROOT_PATH + "assets/skills.json";

  // Paths to generated files
  const std::string GENERATED_AREA_MAP_PATH = GENERATED_ASSETS_PATH + "area_map.txt";
  const std::string GENERATED_ACCESS_MAP_PATH = GENERATED_ASSETS_PATH + "access_map.txt";
  const std::string GENERATED_HEIGHT_MAP_PATH = GENERATED_ASSETS_PATH + "height_map.txt";
  const std::string GENERATED_OBJECT_JSON_PATH = GENERATED_ASSETS_PATH + "objects.json";
  const std::string GENERATED_MAP_PATH = GENERATED_ASSETS_PATH + "map.obj";

  const float HEIGHT_MAP_SCALE = 0.1;
  const float TILE_SIDE_SCALE = 1.0;
  const float TILE_SIDE_UNIT = 1.0;
} Config;