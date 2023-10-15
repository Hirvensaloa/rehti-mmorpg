#include "GameWorld.hpp"
#include "entity/Bandit.hpp"
#include "entity/Goblin.hpp"

GameWorld::GameWorld(){};

void GameWorld::addPlayer(std::string playerName, unsigned int playerId, Coordinates location)
{
    PlayerCharacter newPlayer = PlayerCharacter(this, playerName, playerId, location);
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

PlayerCharacter *GameWorld::getPlayer(unsigned int playerId)
{
    for (auto it = playersM.begin(); it != playersM.end(); it++)
    {
        if (it->getId() == playerId)
        {

            return &(*it);
        }
    }
    return nullptr;
}

void GameWorld::addNpc(Npc npc)
{
    npcsM.push_back(std::make_shared<Npc>(npc));
}

std::vector<std::shared_ptr<Npc>> &GameWorld::getNpcs()
{
    return npcsM;
}

void GameWorld::updateGameWorld()
{
    for (auto npc : npcsM)
    {
        npc->update();
    }
    for (PlayerCharacter &p : playersM)
    {
        p.update();
    }
}

void GameWorld::initWorld()
{
    npcsM.push_back(std::make_shared<Goblin>(this, "Kimmo-Goblin", 1337, Coordinates(1, 1)));
    npcsM.push_back(std::make_shared<Bandit>(this, "Roisto-Pena", 123, Coordinates(5, 5)));
}