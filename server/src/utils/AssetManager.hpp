#pragma once

#include <map>
#include <memory>

#include "../item/Item.hpp"
#include "RehtiReader.hpp"

/**
 * @brief Holds definitions to all the items, objects and skills in the game.
 * This class is used to fetch the definitions and create instances of the items, objects and skills.
 */
class AssetManager
{
public:
    /**
     * @brief Loads all the assets into memory.
     *
     */
    static void loadAssets();

    /**
     * @brief Get the Object Locations object
     *
     * @return std::vector<ObjectLocation>
     */
    static const std::vector<ObjectLocation>& getObjectLocations();

    /**
     * @brief Get the Skills object
     *
     * @return std::map<int, GameSkill>
     */
    static const std::map<int, GameSkill>& getSkills();

    /**
     * @brief Get the GameObjects object
     *
     * @return GameObjects
     */
    static const GameObjects& getObjects();

    /**
     * @brief Get the GameItems object
     *
     * @return GameItems
     */
    static const GameItems& getItems();

    /**
     * @brief Get the GameCharacters object
     *
     * @return GameCharacters
     */
    static const GameCharacters& getGameCharacters();

    /**
     * @brief Creates an instance of an item.
     *
     * @param id Item id
     * @return std::shared_ptr<Item>, nullptr if item id is not found.
     */
    static std::shared_ptr<Item> createItemInstance(int id);

private:
    inline static GameItems itemsM = {};
    inline static std::map<int, GameSkill> skillsM = {};
    inline static GameObjects objectsM = {};                         ///< Tells what possible objects exists and what attributes they have (For example tree)
    inline static std::vector<ObjectLocation> objectLocationsM = {}; ///< Tells where objects instances are located on the map (For example, tree at 1,1, tree at 2,2, tree at 4,5 etc)
    inline static GameCharacters gameCharactersM = {};
};