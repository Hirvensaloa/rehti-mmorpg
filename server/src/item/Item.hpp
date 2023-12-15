#pragma once

#include <string>

#include "RehtiReader.hpp"

class Entity;

/**
 * @brief Base class for items
 */
class Item
{
public:
    Item(int id, std::string name, bool isStackable = false);

    Item(const GeneralItemStruct& generalItem);

    /**
     * @brief Get the type id of the item
     * @return const int&
     */
    const int& getId();

    /**
     * @brief Get the instance id of the item
     * @return const int&
     */
    const int& getInstanceId();

    /**
     * @brief Get the stack size of the item
     * @return const unsigned int&
     */
    const unsigned int& getStackSize();

    /**
     * @brief Get the name of the item
     * @return const std::string&
     */
    const std::string& getName();

    /**
     * @brief Get whether the item is stackable or not
     * @return const bool&
     */
    const bool& IsStackable();

    /**
     * @brief Function for using the item. This base class of items cannot be used,
     * but derived classes should override this function
     * @param user that uses the item
     * @return bool (if returns true, item should be deleted)
     */
    virtual bool use(Entity* user);

protected:
    int idM;
    int instanceIdM;

    const unsigned int stackSizeM = 1; ///< TODO: Implement amount functionality for stackable items

    std::string nameM;

    bool isStackableM;

    inline static int nextInstanceIdM = 0;
};