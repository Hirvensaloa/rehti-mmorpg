#pragma once

#include "Npc.hpp"

class PlayerCharacter;

class Bandit : public Npc
{

public:
    Bandit(GameWorld *pGameWorld, std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~Bandit() = default;

    void update() override;

    std::shared_ptr<PlayerCharacter> findClosestPlayer();
};