#pragma once

#include <vector>

#include "PlayerCharacter.hpp"

class GameWorld
{

public:
    GameWorld();

    ~GameWorld() = default;

    void addPlayer(std::string playerName, unsigned int playerId, Coordinates location);

    bool removePlayer(unsigned int playerId);

    std::vector<PlayerCharacter> &getPlayers();

private:
    std::vector<PlayerCharacter> playersM;
};
