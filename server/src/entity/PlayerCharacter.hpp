#pragma once

#include <string>

#include "../skill/SkillSet.hpp"
#include "Entity.hpp"

class PlayerCharacter : public Entity
{
public:
    PlayerCharacter(GameWorld* pGameWorld, std::string name, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, unsigned int id = 0, Coordinates location = Coordinates());

    ~PlayerCharacter() = default;

    /**
     * @brief Calls act() function for player's current action, if the player has one. Update is called when server ticks.
     */
    void update();

    /**
     * @brief Function that drops player's items and sets an active RespawnAction. Called when player dies.
     */
    void respawn();

    /**
     * @brief Performs player's attack on target entity. Calculates hit chance and damage, and on succesful hit applies the damage and xp gained from the hit.
     * @param target
     */
    void attack(Entity& target) override;
};