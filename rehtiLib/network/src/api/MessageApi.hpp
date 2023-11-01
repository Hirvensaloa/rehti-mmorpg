#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>

#include "Types.hpp"

/*
 * CREATING NEW MESSAGES:
 *
 * 1. Add a new enum value to MessageId enum.
 * 2. Create a new struct for the message type.
 * 3. Add a new create and parse method to MessageApi class.
 * 4. Handle the message instance on the server
 */

enum MessageId
{
    GameState,
    Move,
    Attack,
    ObjectInteract,
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

struct GameStateMessage
{
    const MessageId id = MessageId::GameState;
    std::vector<GameStateEntity> entities;
    std::vector<GameStateObject> objects;
    CurrentPlayer currentPlayer;
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

struct ObjectInteractMessage
{
    const MessageId id = MessageId::ObjectInteract;
    std::string objectId;
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

    static MessageStruct createObjectInteract(const ObjectInteractMessage &objectInteract);
    static ObjectInteractMessage parseObjectInteract(std::string msgBody);

    static MessageStruct createGameState(const GameStateMessage &gameState);
    static GameStateMessage parseGameState(std::string msgBody);
};