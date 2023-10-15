#pragma once

#include "AttackAction.hpp"
#include "Coordinates.hpp"
#include "MoveAction.hpp"

class GameWorld;

class Entity
{
public:
    Entity(GameWorld *pGameWorld, std::string name, unsigned int id = 0, Coordinates location = Coordinates());

    ~Entity() = default;

    unsigned int getId();

    std::string getName();

    Coordinates &getLocation();

    Action &getCurrentAction();

    unsigned int getHp();

    void changeHp(int amount);

    void setAction(std::shared_ptr<Action> action);

    void move(Coordinates location);

    void attack(Entity &target);

    virtual void update() = 0;

protected:
    unsigned int idM;

    std::string nameM;

    Coordinates locationM;

    std::shared_ptr<Action> currentActionM;

    unsigned int hpM = 10;

    GameWorld *pGameWorldM;
};