#include "Bandit.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "../world/GameWorld.hpp"

#include <iostream>

Bandit::Bandit(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : Npc(pGameWorld, name, id, location){};

void Bandit::update()
{
    // Bandits are aggressive and will attack the nearest player
    if (currentActionM && !currentActionM->isCompleted())
    {
        currentActionM->act();
    }
    else
    {
        std::shared_ptr<PlayerCharacter> target = findClosestPlayer();
        if (target != nullptr)
        {
            std::cout << "closest player: " << target->getName() << std::endl;
            setAction(std::make_shared<AttackAction>(std::chrono::system_clock::now(), target, this->shared_from_this()));
        }
    }
};

std::shared_ptr<PlayerCharacter> Bandit::findClosestPlayer()
{
    unsigned int smallestDistance = 9999;
    std::shared_ptr<PlayerCharacter> closestPlayer = nullptr;
    for (int i = 0; i < pGameWorldM->getPlayers().size(); i++)
    {
        std::shared_ptr<PlayerCharacter> player = pGameWorldM->getPlayers()[i];
        if (player->getHp() != 0)
        {
            unsigned int dist = locationM.distance(player->getLocation());
            if (dist < smallestDistance)
            {
                closestPlayer = player;
                smallestDistance = dist;
            }
        }
    }
    return closestPlayer;
};
