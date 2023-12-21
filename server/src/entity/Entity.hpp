#pragma once

#include "../action/AttackAction.hpp"
#include "../action/MoveAction.hpp"
#include "../action/RespawnAction.hpp"
#include "../item/Equipment.hpp"
#include "../item/Inventory.hpp"
#include "../skill/SkillSet.hpp"
#include "../world/Coordinates.hpp"

class GameWorld;

/**
 * @brief Base pure virtual Entity class. Different types of enities inherit this.
 */
class Entity : public std::enable_shared_from_this<Entity>
{
public:
    Entity(GameWorld* pGameWorld, std::string name, int baseAccuracy, int baseDamage, SpawnCoordinateBounds spawnCoordinateBounds, unsigned int id = 0, Coordinates location = Coordinates());

    ~Entity() = default;

    /**
     * @brief Get the type id of this entity
     * @return unsigned int
     */
    unsigned int getId();

    /**
     * @brief Get the unique id of this entity
     *
     * @return unsigned int
     */
    unsigned int getInstanceId();

    /**
     * @brief Get the name of this entity
     * @return std::string
     */
    std::string getName();

    /**
     * @brief Get the current location of this entity
     * @return Coordinates&
     */
    Coordinates& getLocation();

    /**
     * @brief Set the location of this entity
     * @param location
     */
    void setLocation(Coordinates location);

    /**
     * @brief Returns a random location within the spawn bounds
     *
     * @return Coordinates
     */
    Coordinates getRespawnLocation();

    /**
     * @brief Get the current action of this entity
     * @return std::shared_ptr<Action>&
     */
    std::shared_ptr<Action>& getCurrentAction();

    /**
     * @brief Get the current hp of this entity
     * @return int
     */
    int getHp();

    /**
     * @brief Get the maximum hp of this entity
     * @return int
     */
    int getMaxHp();

    /**
     * @brief Get the Inventory object of this entity
     * @return Inventory&
     */
    Inventory& getInventory();

    /**
     * @brief Get the Equipment object of this entity
     * @return Equipment&
     */
    Equipment& getEquipment();

    /**
     * @brief Get tha attack range of this entity
     * @return int
     */
    int getRange();

    /**
     * @brief Get the attack speed of this entity in milliseconds
     * @return int
     */
    int getAttackSpeed();

    /**
     * @brief Get the movement speed of this entity
     * @return std::chrono::milliseconds
     */
    std::chrono::milliseconds getMoveSpeed();

    /**
     * @brief Get pointer to the GameWorld that this entity resides in
     * @return GameWorld*
     */
    GameWorld* getGameWorld();

    /**
     * @brief Change the hp of this entity by amount
     * @param amount
     */
    void changeHp(int amount);

    /**
     * @brief Set the current action to parameter action
     * @param action
     */
    void setAction(std::shared_ptr<Action> action);

    /**
     * @brief Move to a neighboring tile
     * @param location
     * @return success
     */
    bool move(Coordinates location);

    /**
     * @brief Perform an attack on target entity
     * @param target
     */
    virtual void attack(Entity& target);

    /**
     * @brief Pick up an item
     * @param itemId instance id of item to be picked up
     */
    void pickUpItem(int itemId, Coordinates itemLocation);

    /**
     * @brief Drop an item
     * @param itemId instance id of item to be dropped
     */
    void dropItem(int itemId);

    /**
     * @brief Get the SkillSet of this entity
     * @return SkillSet&
     */
    SkillSet& getSkillSet();

    /**
     * @brief Pure virtual function that child classes need to implement. Updates the entity on server tick
     */
    virtual void update() = 0;

    /**
     * @brief Pure virtual function that child classes need to implement. Performs entity's respawning
     */
    virtual void respawn() = 0;

    /**
     * @brief Returns whether entity is disconnected, only relevant for players
     * @return bool
     */
    bool isDisconnected();

    /**
     * @brief Sets entity as disconnected, only relevant for players
     */
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

    std::chrono::milliseconds respawnTimeM{2000};

    std::chrono::milliseconds moveSpeedM{1000};
};