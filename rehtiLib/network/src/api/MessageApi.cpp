#pragma once

#include "MessageApi.hpp"

const MessageStruct MessageApi::createMove(const MoveMessage &move)
{
  rapidjson::Document document = createDocument();
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.AddMember("x", move.x, allocator);
  document.AddMember("y", move.y, allocator);

  return MessageStruct{move.id, createString(document)};
};

MoveMessage MessageApi::parseMove(std::string msgBody)
{
  rapidjson::Document document = parseDocument(msgBody);

  if (!document.HasMember("x") || !document.HasMember("y") || !document["x"].IsInt() || !document["y"].IsInt())
  {
    throw std::runtime_error("Invalid move message");
  }

  MoveMessage move;
  move.x = document["x"].GetInt();
  move.y = document["y"].GetInt();

  return move;
};

MessageStruct MessageApi::createAttack(const AttackMessage &attack)
{
  rapidjson::Document document = createDocument();
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.AddMember("targetId", attack.targetId, allocator);

  return MessageStruct{attack.id, createString(document)};
};

AttackMessage MessageApi::parseAttack(std::string msgBody)
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

MessageStruct MessageApi::createGameState(const GameStateMessage &gameState)
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

GameStateMessage MessageApi::parseGameState(std::string msgBody)
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
