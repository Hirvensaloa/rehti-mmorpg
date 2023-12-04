#pragma once

#include <string>

#include "Npc.hpp"

class PlayerCharacter;

/**
 * @brief PassiveNpc is a type of Npc that attacks only if provoked.
 */
class PassiveNpc : public Npc
{

public:
    PassiveNpc(GameWorld* pGameWorld, std::string name, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, unsigned int id = 0, Coordinates location = Coordinates());

    ~PassiveNpc() = default;

    /**
     * @brief Updates the PassiveNpc.
     *
     * If the PassiveNpc is not currently performing an action, it will walk around. Unless there is a player attacking it.
     */
    void update() override;

private:
    /**
     * @brief Finds out if there is a player attacking the PassiveNpc.
     *
     * @return Shared pointer to the attacking player. Returns nullptr if there is no player attacking the PassiveNpc.
     */
    std::shared_ptr<PlayerCharacter> findAttackingPlayer();
};