#include "GameWorld.hpp"

GameWorld::GameWorld(){};

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, Coordinates location)
{
    PlayerCharacter newPlayer = PlayerCharacter(playerName);
    playersM.push_back(newPlayer);
}

bool GameWorld::removePlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if (it->getId() == playerId)
        {

            playersM.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<PlayerCharacter> &GameWorld::getPlayers()
{
    return playersM;
}