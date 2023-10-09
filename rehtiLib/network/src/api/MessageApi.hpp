#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>

enum MessageId
{
    GameState,
    Move,
    Attack,
    Test,
};

struct MessageStruct
{
    const MessageId id;
    const std::string body;
};

/*
 * All the possible message types between client and server are defined below as structs.
 * Each struct has an id field that is used to identify the message type.
 * The id field corresponds to the messages header id.
 */

struct GameStateEntity
{
    int entityId;
    std::string name;
    int x;
    int y;
    int z;
    int currentActionType = -1; // -1 means no action
};

struct GameStateMessage
{
    const MessageId id = MessageId::GameState;
    std::vector<GameStateEntity> entities;
};

struct MoveMessage
{
    const MessageId id = MessageId::Move;
    int x;
    int y;
};

struct AttackMessage
{
    const MessageId id = MessageId::Attack;
    int targetId;
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
    static const MessageStruct createMove(const MoveMessage &move);
    static MoveMessage parseMove(std::string msgBody);

    static MessageStruct createAttack(const AttackMessage &attack);
    static AttackMessage parseAttack(std::string msgBody);

    static MessageStruct createGameState(const GameStateMessage &gameState);
    static GameStateMessage parseGameState(std::string msgBody);

private:
    // Helper functions for creating and parsing json documents

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