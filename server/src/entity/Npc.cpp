#include "Npc.hpp"
#include "../utils/AssetManager.hpp"
#include "../world/Map.hpp"

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
      chatResponsesM(chatResponses)
{
    // Add a random item to the NPC's inventory
    const GameItems& gameItems = AssetManager::getItems();
    const std::vector<int> ids = gameItems.getAllIds();
    const int randomId = ids[rand() % ids.size()];
    const std::shared_ptr<Item> item = AssetManager::createItemInstance(randomId);
    getInventory().addItem(item);
};

void Npc::update()
{
    if (currentActionM != nullptr && !currentActionM->isCompleted())
    {
        currentActionM->act();
    }
    else
    {
        // By default, NPCs just walk around
        const int notWalk = rand() % 10;
        if (!notWalk)
        {
            setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Map::getRandomCoordinates(this->spawnCoordinateBoundsM), this->shared_from_this()));
        }
    }
}

void Npc::respawn()
{
    for (auto item : getInventory().getItems())
    {
        dropItem(item->getInstanceId());
    }
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
