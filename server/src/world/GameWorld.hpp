#pragma once

#include <vector>
#include <map>

#include "../utils/AssetManager.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../entity/Npc.hpp"
#include "../object/Object.hpp"
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

    std::map<std::string, std::unique_ptr<Object>> &getObjects();

    void updateGameWorld();

    void initWorld();

private:
    std::vector<PlayerCharacter> playersM;
    std::vector<std::shared_ptr<Npc>> npcsM;
    std::map<std::string, std::unique_ptr<Object>> objectsM;
    Map mapM;
    AssetManager assetManagerM;
};
