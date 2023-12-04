#pragma once

#include "../action/AttackAction.hpp"
#include "../action/MoveAction.hpp"
#include "../action/RespawnAction.hpp"
#include "../item/Equipment.hpp"
#include "../item/Inventory.hpp"
#include "../skill/SkillSet.hpp"
#include "../world/Coordinates.hpp"

class GameWorld;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
    Entity(GameWorld* pGameWorld, std::string name, int baseAccuracy, int baseDamage, SpawnCoordinateBounds spawnCoordinateBounds, unsigned int id = 0, Coordinates location = Coordinates());

    ~Entity() = default;

    unsigned int getId();

    /**
     * @brief Get the Instance Id object
     *
     * @return unsigned int
     */
    unsigned int getInstanceId();

    std::string getName();

    Coordinates& getLocation();

    void setLocation(Coordinates& location);

    Coordinates& getRespawnLocation();

    std::shared_ptr<Action>& getCurrentAction();

    int getHp();

    int getMaxHp();

    Inventory& getInventory();

    Equipment& getEquipment();

    int getRange();

    int getAttackSpeed();

    GameWorld* getGameWorld();

    void changeHp(int amount);

    void setAction(std::shared_ptr<Action> action);

    bool move(Coordinates location);

    void attack(Entity& target);

    SkillSet& getSkillSet();

    virtual void update() = 0;

    virtual void respawn() = 0;

    bool isDisconnected();

    void setDisconnected();

protected:
    unsigned int idM;
    unsigned int instanceIdM;
    inline static int nextInstanceIdM = 0;

    std::string nameM;

    Coordinates locationM;

    SpawnCoordinateBounds spawnCoordinateBoundsM;

    std::shared_ptr<Action> currentActionM = nullptr;

    int maxHpM = 1000;

    int hpM = maxHpM;

    GameWorld* pGameWorldM;

    Inventory inventoryM;

    Equipment equipmentM;

    SkillSet skillSetM;

    int baseDamageM;
    int baseAccuracyM;

    bool isDisconnectedM = false; // for players only, but defined for entity for polymorphism reaons

    std::chrono::milliseconds respawnTimeM{10000};
};