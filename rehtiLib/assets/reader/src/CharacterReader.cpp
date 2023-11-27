#include "CharacterReader.hpp"
#include "../../Config.hpp"
#include "RehtiUtils.hpp"
#include <stdexcept>
/**
 * @brief Reads the character data from JSON file.
 * @return GameCharacters
 */
GameCharacters readCharacterData()
{
    GameCharacters gameCharacters;

    rapidjson::Document document = readJson(Config.CHARACTER_JSON_PATH);

    if (!document.IsObject() || !validMember(document, "player", ValueType::OBJECT) || !validMember(document, "npcs", ValueType::ARRAY))
    {
        throw std::runtime_error("Invalid JSON structure. Character JSON file is malformed.");
    }

    const rapidjson::Value& player = document["player"];

    if (!validMember(player, "id", ValueType::INT) || !validMember(player, "textureFilename", ValueType::STRING) || !validMember(player, "glTFFilename", ValueType::STRING))
    {
        throw std::runtime_error("Invalid JSON structure. Player JSON object is malformed.");
    }

    gameCharacters.player.id = player["id"].GetInt();
    gameCharacters.player.textureFilename = player["textureFilename"].GetString();
    gameCharacters.player.glTFFilename = player["glTFFilename"].GetString();

    const rapidjson::Value& npcs = document["npcs"];

    for (const auto& npc : npcs.GetArray())
    {
        if (!validMember(npc, "id", ValueType::INT) || !validMember(npc, "textureFilename", ValueType::STRING) || !validMember(npc, "glTFFilename", ValueType::STRING))
        {
            throw std::runtime_error("Invalid JSON structure. NPC JSON object is malformed.");
        }

        NPC npcData;
        npcData.id = npc["id"].GetInt();
        npcData.textureFilename = npc["textureFilename"].GetString();
        npcData.glTFFilename = npc["glTFFilename"].GetString();

        gameCharacters.npcs.push_back(npcData);
    }

    return gameCharacters;
}