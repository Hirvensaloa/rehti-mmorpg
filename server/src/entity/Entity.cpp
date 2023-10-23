#include <iostream>

#include "../world/GameWorld.hpp"
#include "Entity.hpp"

Entity::Entity(GameWorld *pGameWorld, std::string name, unsigned int id, Coordinates location) : idM(id), nameM(name), locationM(location), pGameWorldM(pGameWorld), inventoryM(Inventory(this)), equipmentM(Equipment(this)){};

unsigned int Entity::getId()
{
    return idM;
}

std::string Entity::getName()
{
    return nameM;
}

Coordinates &Entity::getLocation()
{
    return locationM;
}

Action &Entity::getCurrentAction()
{
    return *currentActionM;
}

unsigned int Entity::getHp()
{
    return hpM;
}

Inventory &Entity::getInventory()
{
    return inventoryM;
}

Equipment &Entity::getEquipment()
{
    return equipmentM;
}

int Entity::getRange()
{
    return std::max(1, equipmentM.getEquipmentStats().range);
}

int Entity::getAttackSpeed()
{
    int equipmentAttackSpeed = equipmentM.getEquipmentStats().attackSpeed;
    return equipmentAttackSpeed != 0 ? equipmentAttackSpeed : 2000;
}

GameWorld *Entity::getGameWorld()
{
    return pGameWorldM;
}

void Entity::changeHp(int amount)
{
    if (hpM > -amount)
    {
        hpM += amount;
    }
    else
    {
        hpM = 0;
    }
}

void Entity::setAction(std::shared_ptr<Action> action)
{
    currentActionM = action;
}

bool Entity::move(Coordinates target)
{
    if (locationM.distance(target) < 2)
    {
        locationM = target;
        return true;
    }
    return false;
}

void Entity::attack(Entity &target)
{
    int baseDamage = 10;   // Base damage will be calculated based on skills later, placeholder value for now
    int baseAccuracy = 50; // Same here

    int totalAccuracy = getEquipment().getEquipmentStats().accuracy + baseAccuracy - target.getEquipment().getEquipmentStats().dodge;
    int totalDamage = getEquipment().getEquipmentStats().damage + baseDamage - target.getEquipment().getEquipmentStats().armor;

    bool hitConnects = totalAccuracy > rand() % 100;

    if (hitConnects)
    {
        unsigned int damage = totalDamage;
        target.changeHp(-damage);
        std::cout << "Entity " << target.getName() << " took " << damage << " damage from " << getName() << ". Remaining HP: " << target.getHp() << std::endl;
    }
    else
    {
        std::cout << "Entity " << getName() << " missed an attack on " << target.getName() << "." << std::endl;
    }
}