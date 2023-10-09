#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <iostream>

#include "MessageTypes.hpp"

struct MessageStruct
{
  const int id;
  const std::string body;
};

/*
 * Helper class for constructing and parsing messages.
 * Each message type above needs to have create and parse methods.
 *
 * The create methods take the message parameters as arguments and return a Message header id and Message body.
 *
 * The parse methods take a Message body string as an argument and return the message parameters.
 * Parse function might fail if the message body is not a valid json string. In that case an exception is thrown.
 */
class MessageApi
{
public:
  static const MessageStruct createMove(const MoveMessage &move)
  {
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
    document.AddMember("x", move.x, allocator);
    document.AddMember("y", move.y, allocator);

    return MessageStruct{move.id, createString(document)};
  };

  static MoveMessage parseMove(std::string msgBody)
  {
    rapidjson::Document document = parseDocument(msgBody);

    if (!document.HasMember("x") || !document.HasMember("y") || !document.HasMember("z") || !document["y"].IsInt())
    {
      throw std::runtime_error("Invalid move message");
    }

    MoveMessage move;
    move.x = document["x"].GetInt();
    move.y = document["y"].GetInt();

    return move;
  };

  static MessageStruct createAttack(const AttackMessage &attack)
  {
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
    document.AddMember("targetId", attack.targetId, allocator);

    return MessageStruct{attack.id, createString(document)};
  };

  static AttackMessage parseAttack(std::string msgBody)
  {
    rapidjson::Document document = parseDocument(msgBody);

    if (!document.HasMember("targetId") || !document["targetId"].IsInt())
    {
      throw std::runtime_error("Invalid attack message");
    }

    AttackMessage attack;
    attack.targetId = document["targetId"].GetInt();

    return attack;
  };

  static MessageStruct createGameState(const GameStateMessage &gameState)
  {
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
    rapidjson::Value entityArray(rapidjson::kArrayType);

    for (const auto &entity : gameState.entities)
    {
      rapidjson::Value entityObject(rapidjson::kObjectType);
      entityObject.AddMember("entityId", entity.entityId, allocator);
      entityObject.AddMember("x", entity.x, allocator);
      entityObject.AddMember("y", entity.y, allocator);
      entityObject.AddMember("z", entity.z, allocator);
      entityObject.AddMember("currentActionType", entity.currentActionType, allocator);
      entityArray.PushBack(entityObject, allocator);
    }
    document.AddMember("entities", entityArray, allocator);

    return MessageStruct{gameState.id, createString(document)};
  };

  static GameStateMessage parseGameState(std::string msgBody)
  {
    rapidjson::Document document = parseDocument(msgBody);

    // No need to validate the message body here, since the server is the only one sending game state messages.
    // The server is trusted to send valid messages.

    GameStateMessage gameState;
    gameState.entities = std::vector<GameStateEntity>();

    for (const auto &entity : document["entities"].GetArray())
    {
      GameStateEntity gameStateEntity;
      gameStateEntity.entityId = entity["entityId"].GetInt();
      gameStateEntity.x = entity["x"].GetInt();
      gameStateEntity.y = entity["y"].GetInt();
      gameStateEntity.z = entity["z"].GetInt();
      gameStateEntity.currentActionType = entity["currentActionType"].GetInt();

      gameState.entities.push_back(gameStateEntity);
    }

    return gameState;
  };

private:
  static rapidjson::Document createDocument()
  {
    rapidjson::Document document;
    document.SetObject();

    return document;
  }

  /*
   * Parses a string into a rapidjson document.
   * Throws an exception if parsing fails.
   */
  static rapidjson::Document parseDocument(std::string str)
  {
    rapidjson::Document document;
    document.Parse(str.c_str());

    if (document.HasParseError())
    {
      std::cout << "Error parsing json: " << document.GetParseError() << std::endl;
      throw std::runtime_error("Error parsing json");
    }

    return document;
  }

  /*
   * Creates a string from a rapidjson document.
   */
  static std::string createString(rapidjson::Document &document)
  {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    const std::string str = buffer.GetString();
    return str;
  }
};