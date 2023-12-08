#include "AggressiveNpc.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

AggressiveNpc::AggressiveNpc(
    GameWorld* pGameWorld,
    std::string name,
    int aggressionRange,
    int baseDamage,
    int baseAccuracy,
    SpawnCoordinateBounds spawnCoordinateBounds,
    std::vector<std::string> chatResponses,
    unsigned int id,
    Coordinates location)
    : Npc(
          pGameWorld,
          name,
          baseDamage,
          baseAccuracy,
          spawnCoordinateBounds,
          chatResponses,
          id,
          location),
      aggressionRangeM(aggressionRange){};

void AggressiveNpc::update()
{
    if (currentActionM && !currentActionM->isCompleted())
    {
        currentActionM->act();
    }
    else
    {
        std::shared_ptr<PlayerCharacter> target = findClosestPlayer();
        if (target != nullptr)
        {
            setAction(std::make_shared<AttackAction>(std::chrono::system_clock::now(), target, this->shared_from_this()));
        }
    }
};

std::shared_ptr<PlayerCharacter> AggressiveNpc::findClosestPlayer()
{
    std::shared_ptr<PlayerCharacter> closestPlayer = nullptr;
    for (int i = 0; i < pGameWorldM->getPlayers().size(); i++)
    {
        std::shared_ptr<PlayerCharacter> player = pGameWorldM->getPlayers()[i];
        if (player->getHp() != 0)
        {
            unsigned int dist = locationM.distance(player->getLocation());
            if (dist < aggressionRangeM)
            {
                closestPlayer = player;
                aggressionRangeM = dist;
            }
        }
    }
    return closestPlayer;
};
