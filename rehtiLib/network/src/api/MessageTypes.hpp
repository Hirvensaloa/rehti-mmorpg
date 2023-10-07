#pragma once

#include <optional>
#include <vector>

enum MessageId
{
    GameState,
    Move,
    Attack,
    Test,
};

/*
 * All the possible message types between client and server are defined below as structs.
 * Each struct has an id field that is used to identify the message type.
 * The id field corresponds to the messages header id.
 */

struct GameStateEntity
{
    int entityId;
    int x;
    int y;
    int z;
    int currentActionId = -1; // -1 means no action
};

struct GameStateMessage
{
    int id = MessageId::GameState;
    std::vector<GameStateEntity> entities;
};

struct MoveMessage
{
    int id = MessageId::Move;
    int x;
    int y;
    int z;
};

struct AttackMessage
{
    int id = MessageId::Attack;
    int targetId;
};