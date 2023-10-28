#pragma once

#include <string>
#include <vector>

/*
 * This file contains the sub-types that are used when defining Messages. Made to avoid bloating the MessageApi file
 */

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
  int currentActionType = -1; // -1 means no action
};

struct Skill
{
  int id;
  std::string name;
  int xp;
};

// Current player (The player who receives the message). Contains more info that is not exposed to other players.
struct CurrentPlayer
{
  int entityId;
  std::string name;
  int x;
  int y;
  int z;
  int currentActionType = -1; // -1 means no action
  std::vector<Skill> skills;
};