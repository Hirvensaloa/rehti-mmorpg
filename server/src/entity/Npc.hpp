#pragma once

#include <string>

#include "Entity.hpp"

/**
 * @brief Base NPC class. It represents a peaceful NPC that does not attack the player under any circumstances.
 */
class Npc : public Entity
{

public:
    Npc(GameWorld* pGameWorld, std::string name, int baseDamage, int baseAccuracy, SpawnCoordinateBounds spawnCoordinateBounds, std::vector<std::string> chatResponses, unsigned int id = 0, Coordinates location = Coordinates());

    ~Npc() = default;

    /**
     * @brief Updates the NPC.
     *
     * If the NPC is not currently performing an action, it will walk around.
     */
    void update();

    /**
     * @brief Respawn the NPC.
     *
     * The NPC will respawn after a certain amount of time.
     */
    void respawn();

    /**
     * @brief Get a random chat response.
     *
     * @return std::string. Returns an empty string if the NPC has no chat responses.
     */
    std::string getChatResponse();

private:
    std::vector<std::string> chatResponsesM;
};