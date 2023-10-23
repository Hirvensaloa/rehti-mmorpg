#pragma once

#include "../action/AttackAction.hpp"
#include "../action/MoveAction.hpp"
#include "../item/Equipment.hpp"
#include "../item/Inventory.hpp"
#include "../world/Coordinates.hpp"

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

    Inventory &getInventory();

    Equipment &getEquipment();

    int getRange();

    int getAttackSpeed();

    GameWorld *getGameWorld();

    void changeHp(int amount);

    void setAction(std::shared_ptr<Action> action);

    bool move(Coordinates location);

    void attack(Entity &target);

    virtual void update() = 0;

protected:
    unsigned int idM;

    std::string nameM;

    Coordinates locationM;

    std::shared_ptr<Action> currentActionM;

    unsigned int hpM = 1000;

    GameWorld *pGameWorldM;

    Inventory inventoryM;

    Equipment equipmentM;
};