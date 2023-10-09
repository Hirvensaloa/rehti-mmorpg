#pragma once

#include <vector>

#include "PlayerCharacter.hpp"
#include "entity/Npc.hpp"

class GameWorld
{

public:
    GameWorld();

    ~GameWorld() = default;

    void addPlayer(std::string playerName, unsigned int playerId, Coordinates location);

    bool removePlayer(unsigned int playerId);

    std::vector<PlayerCharacter> &getPlayers();

    PlayerCharacter *getPlayer(unsigned int playerId);

    void addNpc(Npc npc);

    std::vector<Npc> &getNpcs();

    void updateGameWorld();

    void initWorld();

private:
    std::vector<PlayerCharacter> playersM;
    std::vector<Npc> npcsM;
};
