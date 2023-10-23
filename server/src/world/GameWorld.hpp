#pragma once

#include <vector>

#include "../entity/Npc.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "Map.hpp"

class GameWorld
{

public:
    GameWorld();

    ~GameWorld() = default;

    std::vector<PlayerCharacter> &getPlayers();

    PlayerCharacter *getPlayer(unsigned int playerId);

    Map &getMap();

    void addPlayer(std::string playerName, unsigned int playerId, Coordinates location);

    bool removePlayer(unsigned int playerId);

    void addNpc(Npc npc);

    std::vector<std::shared_ptr<Npc>> &getNpcs();

    void updateGameWorld();

    void initWorld();

private:
    std::vector<PlayerCharacter> playersM;

    std::vector<std::shared_ptr<Npc>> npcsM;

    Map mapM;
};
