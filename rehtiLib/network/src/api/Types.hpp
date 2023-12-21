#pragma once

#include <iostream>
#include <string>
#include <vector>

/**
 * @file This file contains the sub-types that are used when defining Messages. Made to avoid bloating the MessageApi file
 */

/**
 * @brief All the possible message ids
 */
enum class MessageId
{
    GameState,
    Login,
    Move,
    Attack,
    ObjectInteract,
    Test,
    Informative,
    UseItem,
    Unequip,
    DropItem,
    Talk,
    PickUpItem
};

enum class ActionType
{
    None = -1,
    Move = static_cast<int>(MessageId::Move),
    Attack = static_cast<int>(MessageId::Attack),
    ObjectInteract = static_cast<int>(MessageId::ObjectInteract),
    Respawn
};

// Game world objects (Trees, houses etc.)
struct GameStateObject
{
    int id;
    std::string instanceId;
    std::string name;
    int x;
    int y;
    int z;
    unsigned rotation;
};

struct Coordinate
{
    int x;
    int y;
    int z;
};

inline bool operator<(const Coordinate& l, const Coordinate& r)
{
    return std::tie(l.x, l.y, l.z) < std::tie(r.x, r.y, r.z);
}

/**
 * @brief CurrentAction struct representing an action that can be performed by an entity.
 */
struct CurrentAction
{
    ActionType id;
    int durationMs;              ///< How long one iteration of the action takes
    bool looping;                ///< Whether the action should be looped or not
    unsigned int targetId;       ///< The action target id. This differs depending on the action type. For example, if the action type is attack, this is the id of the target entity. If it is move then this is undefined.
    Coordinate targetCoordinate; ///< The action target coordinate. Can be used to define the action direction (with the current coordinate of the entity).

    /**
     * @brief Check if two CurrentActions are exactly the same
     *
     * @param other CurrentAction to compare to
     * @return bool
     */
    bool operator==(const CurrentAction& other) const
    {
        return id == other.id && durationMs == other.durationMs && looping == other.looping && targetId == other.targetId && targetCoordinate.x == other.targetCoordinate.x && targetCoordinate.y == other.targetCoordinate.y && targetCoordinate.z == other.targetCoordinate.z;
    }

    /**
     * @brief Print the CurrentAction to the console
     */
    void print() const
    {
        std::cout << "CurrentAction: " << std::endl;
        std::cout << "id: " << static_cast<int>(id) << std::endl;
        std::cout << "durationMs: " << durationMs << std::endl;
        std::cout << "looping: " << looping << std::endl;
        std::cout << "targetId: " << targetId << std::endl;
        std::cout << "targetCoordinate: " << targetCoordinate.x << ", " << targetCoordinate.y << ", " << targetCoordinate.z << std::endl;
    }
};

struct GameItem
{
    int id;
    int instanceId;
    std::string name;
    unsigned int stackSize; // If item is stackable, this is the amount of items in the stack. If not, this is 1.

    /**
     * @brief Check if two GameItems are exactly the same
     *
     * @param other GameItem to compare to
     * @return bool
     */
    bool operator==(const GameItem& other) const
    {
        return id == other.id && instanceId == other.instanceId && name == other.name && stackSize == other.stackSize;
    }
};

// Npc's and other players
struct GameStateEntity
{
    int id;
    int instanceId;
    std::string name;
    int x;
    int y;
    int z;
    int hp;
    CurrentAction currentAction = {ActionType::None};
    std::vector<GameItem> equipment;

    /**
     * @brief Check if id's match
     *
     * @param other GameStateEntity to compare to
     */
    bool operator==(const GameStateEntity& other) const
    {
        return id == other.id && instanceId == other.instanceId;
    }

    /**
     * @brief Check if coordinates match
     *
     * @param other GameStateEntity to compare to
     * @return bool
     */
    bool hasSameLocationAs(const GameStateEntity& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    /**
     * @brief Check if current action is the same
     *
     * @param other GameStateEntity to compare to
     * @return bool
     */
    bool hasSameActionAs(const GameStateEntity& other) const
    {
        return currentAction == other.currentAction;
    }

    /**
     * @brief Check if equipment is the same
     *
     * @param other GameStateEntity to compare to
     * @return bool
     */
    bool hasSameEquipmentAs(const GameStateEntity& other) const
    {
        // Just check the if the indices match. The equipment should be always in the same order.
        for (int i = 0; i < equipment.size(); i++)
        {
            if (!(equipment[i] == other.equipment[i]))
            {
                return false;
            }
        }
        return true;
    }
};

struct Skill
{
    int id;
    std::string name;
    int xp;

    /**
     * @brief Check if two skills are exactly the same
     *
     * @param other Skill to compare to
     * @return bool
     */
    bool operator==(const Skill& other) const
    {
        return id == other.id && name == other.name && xp == other.xp;
    }
};

/**
 * @brief Current player (The player who receives the message). Contains more info which is not exposed to other players.
 */
struct CurrentPlayer : GameStateEntity
{
    std::vector<Skill> skills;
    std::vector<GameItem> inventory;

    /**
     * @brief Check if two CurrentPlayers has the same skills
     *
     * @param other CurrentPlayer to compare to
     * @return bool
     */
    bool hasSameSkillsAs(const CurrentPlayer& other) const
    {
        // Just check the if the indices match. This is enough because the skills are always in the same order.
        for (int i = 0; i < skills.size(); i++)
        {
            if (!(skills[i] == other.skills[i]))
            {
                return false;
            }
        }
    }

    /**
     * @brief Check if two CurrentPlayers has the same inventory
     *
     * @param other CurrentPlayer to compare to
     * @return bool
     */
    bool hasSameInventoryAs(const CurrentPlayer& other) const
    {
        // Just check the if the indices match. If the items are not in the same order, the inventory is not the same. Items need to be redrawn.
        for (int i = 0; i < inventory.size(); i++)
        {
            if (!(inventory[i] == other.inventory[i]))
            {
                return false;
            }
        }
    }
};