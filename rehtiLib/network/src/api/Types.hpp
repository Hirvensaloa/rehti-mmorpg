#pragma once

#include <string>
#include <vector>

/*
 * This file contains the sub-types that are used when defining Messages. Made to avoid bloating the MessageApi file
 */

struct GameItem
{
    int id;
    int instanceId;
    std::string name;
    unsigned int stackSize; // If item is stackable, this is the amount of items in the stack. If not, this is 1.
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

// Npc's and other players
struct GameStateEntity
{
    int entityId;
    std::string name;
    int x;
    int y;
    int z;
    int hp;
    int currentActionType = -1; // -1 means no action
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