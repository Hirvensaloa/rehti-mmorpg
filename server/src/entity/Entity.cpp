#include <iostream>

#include "../world/GameWorld.hpp"
#include "../world/Utils.hpp"
#include "Entity.hpp"

Entity::Entity(
    GameWorld* pGameWorld,
    std::string name,
    int baseAccuracy,
    int baseDamage,
    SpawnCoordinateBounds spawnCoordinateBounds,
    unsigned int id,
    Coordinates location)
    : idM(id),
      nameM(name),
      locationM(location),
      pGameWorldM(pGameWorld),
      inventoryM(Inventory(this)),
      equipmentM(Equipment(this)),
      skillSetM(SkillSet()),
      baseDamageM(baseDamage),
      baseAccuracyM(baseAccuracy),
      spawnCoordinateBoundsM(spawnCoordinateBounds)
{
    instanceIdM = nextInstanceIdM++;
};

unsigned int Entity::getId()
{
    return idM;
}

unsigned int Entity::getInstanceId()
{
    return instanceIdM;
}

std::string Entity::getName()
{
    return nameM;
}

Coordinates& Entity::getLocation()
{
    return locationM;
}

void Entity::setLocation(Coordinates location)
{
    locationM = location;
}

Coordinates Entity::getRespawnLocation()
{
    const auto accessMap = pGameWorldM->getMap().getAccessMap();
    Coordinates coords = getRandomCoordinates(spawnCoordinateBoundsM, accessMap);

    return coords;
}

std::shared_ptr<Action>& Entity::getCurrentAction()
{
    return currentActionM;
}

int Entity::getHp()
{
    return hpM;
}

int Entity::getMaxHp()
{
    return maxHpM;
}

Inventory& Entity::getInventory()
{
    return inventoryM;
}

Equipment& Entity::getEquipment()
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
    return equipmentAttackSpeed > 0 ? equipmentAttackSpeed : 2000;
}

GameWorld* Entity::getGameWorld()
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
        respawn();
    }
}

void Entity::setAction(std::shared_ptr<Action> action)
{
    currentActionM = action;
}

bool Entity::move(Coordinates target)
{
    if (locationM.distance(target) <= 1)
    {
        locationM = target;
        return true;
    }
    return false;
}

void Entity::attack(Entity& target)
{
    int totalAccuracy = getEquipment().getEquipmentStats().accuracy + baseAccuracyM - target.getEquipment().getEquipmentStats().dodge;
    int totalDamage = getEquipment().getEquipmentStats().damage + baseDamageM - target.getEquipment().getEquipmentStats().armor;

    bool hitConnects = totalAccuracy > rand() % 100;

    if (hitConnects)
    {
        totalDamage = std::max(0, totalDamage);
        target.changeHp(-totalDamage);
        std::cout << "Entity " << target.getName() << " took " << totalDamage << " damage from " << getName() << ". Remaining HP: " << target.getHp() << std::endl;
    }
    else
    {
        std::cout << "Entity " << getName() << " missed an attack on " << target.getName() << "." << std::endl;
    }
}

SkillSet& Entity::getSkillSet()
{
    return skillSetM;
}

bool Entity::isDisconnected()
{
    return isDisconnectedM;
}

void Entity::setDisconnected()
{
    isDisconnectedM = true;
}