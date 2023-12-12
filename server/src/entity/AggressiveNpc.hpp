#pragma once

#include "Npc.hpp"

class PlayerCharacter;

/**
 * @brief AggressiveNpc is a type of Npc that attacks the player within its aggression range.
 */
class AggressiveNpc : public Npc
{

public:
    AggressiveNpc(GameWorld* pGameWorld, std::string name, int aggressionRange, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, std::vector<std::string> chatResponses, unsigned int id = 0, Coordinates location = Coordinates());

    ~AggressiveNpc() = default;

    /**
     * @brief Updates the AggressiveNpc.
     *
     * If the AggressiveNpc is not currently performing an action, it will find the closest player and attack it.
     */
    void update() override;

private:
    /**
     * @brief Finds the closest player to the AggressiveNpc.
     *
     * @return std::shared_ptr<PlayerCharacter>
     */
    std::shared_ptr<PlayerCharacter> findClosestPlayer();

    int aggressionRangeM;
};