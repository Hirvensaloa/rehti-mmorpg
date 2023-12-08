#include "Npc.hpp"

#include <iostream>

Npc::Npc(
    GameWorld* pGameWorld,
    std::string name,
    int baseDamage,
    int baseAccuracy,
    SpawnCoordinateBounds spawnCoordinateBounds,
    std::vector<std::string> chatResponses,
    unsigned int id,
    Coordinates location)
    : Entity(pGameWorld,
             name,
             baseDamage,
             baseAccuracy,
             spawnCoordinateBounds,
             id,
             location),
      chatResponsesM(chatResponses){};

void Npc::update()
{
    // By default, NPCs just walk around
    if (currentActionM != nullptr && !currentActionM->isCompleted())
    {
        currentActionM->act();
    }
    else
    {
        const int notWalk = rand() % 10;
        if (!notWalk)
        {
            setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Coordinates(rand() % 10, rand() % 10), this->shared_from_this()));
        }
    }
}

void Npc::respawn()
{
    getInventory().removeAllItems(); // TODO: drop them on the ground
    setAction(std::make_shared<RespawnAction>(std::chrono::system_clock::now(), respawnTimeM, this->shared_from_this()));
}

std::string Npc::getChatResponse()
{
    if (chatResponsesM.empty())
    {
        return "";
    }

    return chatResponsesM[rand() % chatResponsesM.size()];
}
