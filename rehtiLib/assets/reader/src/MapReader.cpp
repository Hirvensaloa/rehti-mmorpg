#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "../../Config.hpp"
#include "MapReader.hpp"
#include "Utils.hpp"

std::vector<std::vector<uint8_t>> fetchAccessMatrix()
{
    std::vector<std::vector<uint8_t>> accessMap;
    std::vector<std::vector<std::string>> accessMapString;
    readMatrixFromFile(accessMapString, Config.GENERATED_ACCESS_MAP_PATH);

    for (auto& row : accessMapString)
    {
        std::vector<uint8_t> rowUint8;
        for (auto& col : row)
        {
            rowUint8.push_back(std::stoi(col));
        }
        accessMap.push_back(rowUint8);
    }

    return accessMap;
}

std::vector<std::vector<int>> fetchHeightMatrix()
{
    std::vector<std::vector<int>> heightMap;
    std::vector<std::vector<std::string>> heightMapString;
    readMatrixFromFile(heightMapString, Config.GENERATED_HEIGHT_MAP_PATH);

    for (auto& row : heightMapString)
    {
        std::vector<int> rowInt;
        for (auto& col : row)
        {
            rowInt.push_back(std::stoi(col));
        }
        heightMap.push_back(rowInt);
    }

    return heightMap;
}

std::vector<std::vector<int>> fetchMapTextureMatrix()
{
    std::vector<std::vector<int>> mapTextureMap;
    std::vector<std::vector<std::string>> mapTextureMapString;
    readMatrixFromFile(mapTextureMapString, Config.GENERATED_MAP_TEXTURE_MAP_PATH);

    for (auto& row : mapTextureMapString)
    {
        std::vector<int> rowInt;
        for (auto& col : row)
        {
            rowInt.push_back(std::stoi(col));
        }
        mapTextureMap.push_back(rowInt);
    }

    return mapTextureMap;
}

std::vector<std::vector<std::string>> fetchAreaMatrix()
{
    std::vector<std::vector<std::string>> areaMap;
    readMatrixFromFile(areaMap, Config.GENERATED_AREA_MAP_PATH);

    return areaMap;
}
