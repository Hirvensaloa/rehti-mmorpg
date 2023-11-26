#pragma once

#include <string>
#include <vector>

struct Player
{
    int id;
    std::string textureFilename;
    std::string glTFFilename;
};

struct NPC
{
    int id;
    std::string textureFilename;
    std::string glTFFilename;
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