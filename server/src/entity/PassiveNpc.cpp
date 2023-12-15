#include "PassiveNpc.hpp"
#include "../world/GameWorld.hpp"

PassiveNpc::PassiveNpc(
    GameWorld* pGameWorld,
    std::string name,
    int baseDamage,
    int baseAccuracy,
    SpawnCoordinateBounds spawnCoordinateBounds,
    std::vector<std::string> chatResponses,
    unsigned int id,
    Coordinates location)
    : Npc(pGameWorld,
          name,
          baseDamage,
          baseAccuracy,
          spawnCoordinateBounds,
          chatResponses,
          id,
          location){};

void PassiveNpc::update()
{
    if (currentActionM != nullptr && !currentActionM->isCompleted())
    {
        currentActionM->act();
    }
    else
    {
        std::shared_ptr<PlayerCharacter> target = findAttackingPlayer();

        if (target != nullptr)
        {
            setAction(std::make_shared<AttackAction>(std::chrono::system_clock::now(), target, this->shared_from_this()));
        }
        else
        {
            const int notWalk = rand() % 10;
            if (!notWalk)
            {
                setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), Map::getRandomCoordinates(spawnCoordinateBoundsM), this->shared_from_this()));
            }
        }
    }
}

std::shared_ptr<PlayerCharacter> PassiveNpc::findAttackingPlayer()
{
    auto players = pGameWorldM->getPlayers();
    for (int i = 0; i < players.size(); i++)
    {
        std::shared_ptr<PlayerCharacter> player = players[i];
        const auto action = player->getCurrentAction();
        if (action == nullptr)
        {
            continue;
        }
        const CurrentAction actionInfo = action->getActionInfo();

        if (actionInfo.id == ActionType::Attack && actionInfo.targetId == instanceIdM)
        {
            return player;
        }
    }
    return nullptr;
}