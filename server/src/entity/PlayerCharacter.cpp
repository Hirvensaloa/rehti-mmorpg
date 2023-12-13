#include "PlayerCharacter.hpp"

#include <iostream>

PlayerCharacter::PlayerCharacter(
    GameWorld* pGameWorld,
    std::string name,
    int baseDamage,
    int baseAccuracy,
    SpawnCoordinateBounds spawnCoordinateBounds,
    unsigned int id,
    Coordinates location)
    : Entity(pGameWorld,
             name,
             baseDamage,
             baseAccuracy,
             spawnCoordinateBounds,
             id,
             location)
{
    respawnTimeM = std::chrono::milliseconds(2000);
};

void PlayerCharacter::update()
{

    if (currentActionM != nullptr)
    {
        currentActionM->act();
    }
}

void PlayerCharacter::respawn()
{
    getInventory().removeAllItems(); // TODO: Drop the items on the ground, as well as equipment
    setAction(std::make_shared<RespawnAction>(std::chrono::system_clock::now(), respawnTimeM, this->shared_from_this()));
}

void PlayerCharacter::attack(Entity& target)
{
    const int accuracySkillIndex = 4;
    const int damageSkillIndex = 5;
    const int defenceSkillIndex = 6;

    int totalAccuracy = getSkillSet().getSkillLevel(accuracySkillIndex).value_or(0) + getEquipment().getEquipmentStats().accuracy + baseAccuracyM - target.getEquipment().getEquipmentStats().dodge - target.getSkillSet().getSkillLevel(defenceSkillIndex).value_or(0);
    int totalDamage = getSkillSet().getSkillLevel(damageSkillIndex).value_or(0) + getEquipment().getEquipmentStats().damage + baseDamageM - target.getEquipment().getEquipmentStats().armor;

    bool hitConnects = totalAccuracy > rand() % 100;

    if (hitConnects)
    {
        totalDamage = std::max(0, totalDamage);
        target.changeHp(-totalDamage);
        getSkillSet().addSkillXp(accuracySkillIndex, totalDamage);
        getSkillSet().addSkillXp(damageSkillIndex, totalDamage);
        getSkillSet().addSkillXp(defenceSkillIndex, totalDamage);
        std::cout << "Entity " << target.getName() << " took " << totalDamage << " damage from " << getName() << ". Remaining HP: " << target.getHp() << std::endl;
    }
    else
    {
        std::cout << "Entity " << getName() << " missed an attack on " << target.getName() << "." << std::endl;
    }
}