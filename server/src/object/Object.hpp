#pragma once

#include <string>

#include "../entity/Entity.hpp"
#include "RehtiReader.hpp"

class Object
{
public:
    Object(int id, std::string instanceId, std::string name, Coordinates coordinates, unsigned int rotation, reader::ObjectType objectType);

    Object(const GeneralObjectStruct& object, const ObjectLocation& location, reader::ObjectType objectType);

    virtual ~Object() = default;

    /**
     * @brief Get the id of the object
     *
     * @return const int&
     */
    const int& getId();

    /**
     * @brief Get the instance id of the object
     *
     * @return const std::string&
     */
    const std::string& getInstanceId();

    /**
     * @brief Get the name of the object
     *
     * @return const std::string&
     */
    const std::string& getName();

    /**
     * @brief Get the location of the object
     *
     * @return const Coordinates&
     */
    const Coordinates& getLocation();

    /**
     * @brief Get the rotation of the object
     *
     * @return const unsigned int&
     */
    const unsigned int& getRotation();

    /**
     * @brief Get the ObjectType of object
     *
     * @return const reader::ObjectType&
     */
    const reader::ObjectType& getObjectType();

    /**
     * @brief Interact with the object.
     * @param entity that interacts with the object.
     */
    virtual void interact(Entity& entity) { std::cout << "Cannot interact with this object" << std::endl; };

private:
    int idM;                 ///< Unique id for object type (For example, tree, rock, etc)
    std::string instanceIdM; ///< Unique id for object instance (For example, tree 1, tree at 2 etc)
    std::string nameM;
    Coordinates coordinatesM;
    unsigned int rotationM;
    reader::ObjectType objectTypeM;
};