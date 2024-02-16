#include <filesystem>
#include <iostream>
#include <vector>

#include "src/AnimationTypeGenerator.hpp"
#include "src/MapLoader.hpp"
#include "src/MapObjLoader.hpp"
#include "src/Utils.hpp"

int main()
{
    // Create generated folder if it does not exist. Else remove and create it again. (This is to ensure generated assets don't bloat)
    if (!std::filesystem::exists(Config.GENERATED_ASSETS_PATH))
    {
        std::filesystem::create_directory(Config.GENERATED_ASSETS_PATH);
    }
    else
    {
        std::filesystem::remove_all(Config.GENERATED_ASSETS_PATH);
        std::filesystem::create_directory(Config.GENERATED_ASSETS_PATH);
    }
    // Create generated area obj folder if it does not exist
    if (!std::filesystem::exists(Config.GENERATED_AREA_OBJ_PATH))
    {
        std::filesystem::create_directory(Config.GENERATED_AREA_OBJ_PATH);
    }

    // Generate animations
    generateAnimationHppFile();

    // Fetch game assets
    std::map<int, GameSkill> gameSkills = fetchSkills();
    GameItems gameItems = fetchItems();
    GameObjects gameObjects = fetchObjects(gameItems, gameSkills);

    // Generate map assets
    const std::vector<std::vector<std::string>> areaMap = fetchAreaMap();
    std::vector<std::vector<int>> heightMatrix;
    std::vector<std::vector<int>> mapTextureMatrix;
    loadHeightAndTextureMap(areaMap, heightMatrix, mapTextureMatrix);
    const std::vector<std::vector<std::string>> objectBlockMap = createObjectBlockMap(areaMap, gameObjects, heightMatrix);

    // Generate access map and height map for server
    const std::vector<std::vector<unsigned>> accessMap = generateAccessMap(heightMatrix, objectBlockMap);
    writeMatrixToFile(accessMap, Config.GENERATED_ACCESS_MAP_PATH);
    writeMatrixToFile(heightMatrix, Config.GENERATED_HEIGHT_MAP_PATH);
    writeMatrixToFile(mapTextureMatrix, Config.GENERATED_MAP_TEXTURE_MAP_PATH);
    writeMatrixToFile(areaMap, Config.GENERATED_AREA_MAP_PATH);

    // Generate obj files of the areas for client
    generateAreaObjs(heightMatrix, areaMap);

    std::cout << "Map assets generated successfully!" << std::endl;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Assets loaded and generated successfully!" << std::endl;

    return 0;
}
