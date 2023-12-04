#pragma once

#include <string>
#include <vector>

/**
 * @file This file contains the sub-types that are used when defining Messages. Made to avoid bloating the MessageApi file
 */

/**
 * @brief All the possible message ids
 */
enum class MessageId
{
    GameState,
    Login,
    Move,
    Attack,
    ObjectInteract,
    Test,
    Informative,
    UseItem,
    Unequip,
    DropItem,
    Talk
};

enum class ActionType
{
    None = -1,
    Move = static_cast<int>(MessageId::Move),
    Attack = static_cast<int>(MessageId::Attack),
    ObjectInteract = static_cast<int>(MessageId::ObjectInteract),
    Respawn
};

// Game world objects (Trees, houses etc.)
struct GameStateObject
{
    int id;
    std::string instanceId;
    std::string name;
    int x;
    int y;
    int z;
    unsigned rotation;
};

struct Coordinate
{
    int x;
    int y;
    int z;
};

/**
 * @brief CurrentAction struct representing an action that can be performed by an entity.
 */
struct CurrentAction
{
    ActionType id;
    int durationMs;              ///< How long one iteration of the action takes
    bool looping;                ///< Whether the action should be looped or not
    unsigned int targetId;       ///< The action target id. This differs depending on the action type. For example, if the action type is attack, this is the id of the target entity. If it is move then this is undefined.
    Coordinate targetCoordinate; ///< The action target coordinate. This differs depending on the action type. For example, if the action type is move, this is the coordinate where the entity is moving to. If it is attack then this is undefined.
};

struct GameItem
{
    int id;
    int instanceId;
    std::string name;
    unsigned int stackSize; // If item is stackable, this is the amount of items in the stack. If not, this is 1.
};

// Npc's and other players
struct GameStateEntity
{
    int id;
    int instanceId;
    std::string name;
    int x;
    int y;
    int z;
    int hp;
    CurrentAction currentAction = {ActionType::None};
    std::vector<GameItem> equipment;
};

struct Skill
{
    int id;
    std::string name;
    int xp;
};

// Current player (The player who receives the message). Contains more info which is not exposed to other players.
struct CurrentPlayer : GameStateEntity
{
    std::vector<Skill> skills;
    std::vector<GameItem> inventory;
};