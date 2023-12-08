#pragma once

#include <string>
#include <vector>

struct SpawnCoordinateBounds
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

const struct
{
    const std::string Passive = "PASSIVE";
    const std::string Aggressive = "AGGRESSIVE";
    const std::string Peaceful = "PEACEFUL";
} AggressionType;

struct Player
{
    int id;
    std::string textureFilename;
    std::string glTFFilename;
    SpawnCoordinateBounds spawnCoordinateBounds;
    int baseDamage;
    int baseAccuracy;
};

struct NPC
{
    int id;
    std::string name;
    std::string textureFilename;
    std::string glTFFilename;
    SpawnCoordinateBounds spawnCoordinateBounds;
    int spawnAmount;
    std::string agressionType;
    int agressionRange;
    int baseDamage;
    int baseAccuracy;
    std::vector<std::string> chatResponses;
};

/**
 * @brief Contains all the character data. This is used to load character assets into memory and spawn characters on the map.
 */
struct GameCharacters
{
    Player player;
    std::vector<NPC> npcs;
};

/**
 * @brief Reads the character data from JSON file.
 * @return GameCharacters
 */
GameCharacters readCharacterData();

/**
 * @brief Check if SpawnCoordinateBounds is valid. Throws error if not.
 * @param spawnCoordinateBounds SpawnCoordinateBounds
 * @param worldWidth in tiles
 * @param worldHeight in tiles
 */
void validSpawnCoordinateBounds(SpawnCoordinateBounds spawnCoordinateBounds, int worldWidth, int worldHeight);