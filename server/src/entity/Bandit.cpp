#include "Bandit.hpp"
#include "../GameWorld.hpp"
#include "../PlayerCharacter.hpp"

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
        PlayerCharacter *target = findClosestPlayer();
        if (target != nullptr)
        {
            std::cout << "closest player: " << target->getName() << std::endl;
            setAction(std::make_shared<AttackAction>(std::chrono::system_clock::now(), target, this));
        }
    }
};

PlayerCharacter *Bandit::findClosestPlayer()
{
    unsigned int smallestDistance = 9999;
    PlayerCharacter *closestEntity = nullptr;
    for (int i = 0; i < pGameWorldM->getPlayers().size(); i++)
    {
        PlayerCharacter &player = pGameWorldM->getPlayers()[i];
        if (player.getHp() != 0)
        {
            unsigned int dist = locationM.distance(player.getLocation());
            if (dist < smallestDistance)
            {
                closestEntity = &player;
                smallestDistance = dist;
            }
        }
    }
    return closestEntity;
};
