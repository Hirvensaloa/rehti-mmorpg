#pragma once

#include <string>
#include <vector>

/**
 * @brief Contains the spawn coordinate bounds for a character.
 */
struct SpawnCoordinateBounds
{
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

/**
 * @brief Has all the possible NPCData aggression types as strings.
 */
const struct
{
    const std::string Passive = "PASSIVE";
    const std::string Aggressive = "AGGRESSIVE";
    const std::string Peaceful = "PEACEFUL";
} AggressionType;

/**
 * @brief Contains all the player data. This is used to load player assets into memory and spawn players on the map.
 */
struct PlayerData
{
    int id;
    std::string textureFilename;
    std::string glTFFilename;
    SpawnCoordinateBounds spawnCoordinateBounds;
    int baseDamage;
    int baseAccuracy;
};

/**
 * @brief Contains all the NPCData data. This is used to load NPCData assets into memory and spawn NPCs on the map.
 */
struct NPCData
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
    PlayerData player;
    std::vector<NPCData> npcs;
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