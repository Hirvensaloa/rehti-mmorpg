#include "CharacterReader.hpp"
#include "../../Config.hpp"
#include "MapReader.hpp"
#include "RehtiUtils.hpp"
#include <stdexcept>

GameCharacters readCharacterData()
{
    // This is just used to check if the spawn coordinate bounds are valid
    const auto accessMatrix = fetchAccessMatrix();
    const int worldHeight = accessMatrix.size();
    int worldWidth = accessMatrix[0].size();
    for (const auto& row : accessMatrix)
    {
        if (row.size() > worldWidth)
        {
            worldWidth = row.size();
        }
    }

    GameCharacters gameCharacters;

    rapidjson::Document document = readJson(Config.CHARACTER_JSON_PATH);

    if (!document.IsObject() || !validMember(document, "player", ValueType::OBJECT) || !validMember(document, "npcs", ValueType::ARRAY))
    {
        throw std::runtime_error("Invalid JSON structure. Character JSON file is malformed.");
    }

    const rapidjson::Value& player = document["player"];

    if (!validMember(player, "id", ValueType::INT) ||
        !validMember(player, "textureFilename", ValueType::STRING) ||
        !validMember(player, "glTFFilename", ValueType::STRING) ||
        !validMember(player, "spawnCoordinateBounds", ValueType::OBJECT) ||
        !validMember(player["spawnCoordinateBounds"], "xMin", ValueType::INT) ||
        !validMember(player["spawnCoordinateBounds"], "xMax", ValueType::INT) ||
        !validMember(player["spawnCoordinateBounds"], "yMin", ValueType::INT) ||
        !validMember(player["spawnCoordinateBounds"], "yMax", ValueType::INT) ||
        !validMember(player, "baseDamage", ValueType::INT) ||
        !validMember(player, "baseAccuracy", ValueType::INT))
    {
        throw std::runtime_error("Invalid JSON structure. Player JSON object is malformed.");
    }

    gameCharacters.player.id = player["id"].GetInt();
    gameCharacters.player.textureFilename = player["textureFilename"].GetString();
    gameCharacters.player.glTFFilename = player["glTFFilename"].GetString();
    gameCharacters.player.spawnCoordinateBounds.xMin = player["spawnCoordinateBounds"]["xMin"].GetInt();
    gameCharacters.player.spawnCoordinateBounds.xMax = player["spawnCoordinateBounds"]["xMax"].GetInt();
    gameCharacters.player.spawnCoordinateBounds.yMin = player["spawnCoordinateBounds"]["yMin"].GetInt();
    gameCharacters.player.spawnCoordinateBounds.yMax = player["spawnCoordinateBounds"]["yMax"].GetInt();
    gameCharacters.player.baseDamage = player["baseDamage"].GetInt();
    gameCharacters.player.baseAccuracy = player["baseAccuracy"].GetInt();

    validSpawnCoordinateBounds(gameCharacters.player.spawnCoordinateBounds, worldWidth, worldHeight);

    const rapidjson::Value& npcs = document["npcs"];

    for (const auto& npc : npcs.GetArray())
    {
        if (!validMember(npc, "id", ValueType::INT) ||
            !validMember(npc, "name", ValueType::STRING) ||
            !validMember(npc, "textureFilename", ValueType::STRING) ||
            !validMember(npc, "glTFFilename", ValueType::STRING) ||
            !validMember(npc, "spawnCoordinateBounds", ValueType::OBJECT) ||
            !validMember(npc["spawnCoordinateBounds"], "xMin", ValueType::INT) ||
            !validMember(npc["spawnCoordinateBounds"], "xMax", ValueType::INT) ||
            !validMember(npc["spawnCoordinateBounds"], "yMin", ValueType::INT) ||
            !validMember(npc["spawnCoordinateBounds"], "yMax", ValueType::INT) ||
            !validMember(npc, "spawnAmount", ValueType::INT) ||
            !validMember(npc, "agressionType", ValueType::STRING) ||
            !validMember(npc, "agressionRange", ValueType::INT) ||
            !validMember(npc, "baseDamage", ValueType::INT) ||
            !validMember(npc, "baseAccuracy", ValueType::INT) ||
            !validMember(npc, "chatResponses", ValueType::ARRAY))
        {
            throw std::runtime_error("Invalid JSON structure. NPC JSON object is malformed.");
        }
        // Check if agressionType is valid
        if (npc["agressionType"].GetString() != AggressionType.Passive && npc["agressionType"].GetString() != AggressionType.Aggressive && npc["agressionType"].GetString() != AggressionType.Peaceful)
        {
            throw std::runtime_error("Invalid JSON structure. NPC agressionType is invalid. Received: " + static_cast<std::string>(npc["agressionType"].GetString()) + ". Expected: " + AggressionType.Passive + ", " + AggressionType.Aggressive + " or " + AggressionType.Peaceful + ".");
        }

        NPC npcData;
        npcData.id = npc["id"].GetInt();
        npcData.name = npc["name"].GetString();
        npcData.textureFilename = npc["textureFilename"].GetString();
        npcData.glTFFilename = npc["glTFFilename"].GetString();
        npcData.spawnCoordinateBounds.xMin = npc["spawnCoordinateBounds"]["xMin"].GetInt();
        npcData.spawnCoordinateBounds.xMax = npc["spawnCoordinateBounds"]["xMax"].GetInt();
        npcData.spawnCoordinateBounds.yMin = npc["spawnCoordinateBounds"]["yMin"].GetInt();
        npcData.spawnCoordinateBounds.yMax = npc["spawnCoordinateBounds"]["yMax"].GetInt();
        npcData.spawnAmount = npc["spawnAmount"].GetInt();
        npcData.agressionType = npc["agressionType"].GetString();
        npcData.agressionRange = npc["agressionRange"].GetInt();
        npcData.baseDamage = npc["baseDamage"].GetInt();
        npcData.baseAccuracy = npc["baseAccuracy"].GetInt();

        validSpawnCoordinateBounds(npcData.spawnCoordinateBounds, worldWidth, worldHeight);

        for (const auto& chatResponse : npc["chatResponses"].GetArray())
        {
            if (!chatResponse.IsString())
            {
                throw std::runtime_error("Invalid JSON structure. NPC chat response is malformed.");
            }

            npcData.chatResponses.push_back(chatResponse.GetString());
        }

        gameCharacters.npcs.push_back(npcData);
    }

    return gameCharacters;
}

void validSpawnCoordinateBounds(SpawnCoordinateBounds spawnCoordinateBounds, int worldWidth, int worldHeight)
{
    if (spawnCoordinateBounds.xMin > spawnCoordinateBounds.xMax)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds xMin is greater than xMax.");
    }
    if (spawnCoordinateBounds.yMin > spawnCoordinateBounds.yMax)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds yMin is greater than yMax.");
    }
    if (spawnCoordinateBounds.xMin < 0)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds xMin is less than 0.");
    }
    if (spawnCoordinateBounds.yMin < 0)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds yMin is less than 0.");
    }
    if (spawnCoordinateBounds.xMax > worldWidth)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds xMax is greater than world width. World width: " + std::to_string(worldWidth) + ". Received: " + std::to_string(spawnCoordinateBounds.xMax) + ".");
    }
    if (spawnCoordinateBounds.yMax > worldHeight)
    {
        throw std::runtime_error("Invalid JSON structure. SpawnCoordinateBounds yMax is greater than world height. World height: " + std::to_string(worldHeight) + ". Received: " + std::to_string(spawnCoordinateBounds.yMax) + ".");
    }
}