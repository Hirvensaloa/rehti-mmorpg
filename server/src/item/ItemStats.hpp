#pragma once

#include <chrono>
#include <ostream>

struct ItemStats
{
    ItemStats(int acc = 0, int dam = 0, int aspd = 0, int rng = 0, int arm = 0, int dod = 0) : accuracy(acc), damage(dam), attackSpeed(aspd), range(rng), armor(arm), dodge(dod){};

    int accuracy;

    int damage;

    int attackSpeed;

    int range;

    int armor;

    int dodge;

    ItemStats operator+(ItemStats other)
    {
        return ItemStats(accuracy + other.accuracy, damage + other.damage, std::max(attackSpeed, other.attackSpeed), std::max(range, other.range), armor + other.armor, dodge + other.dodge);
    }

    friend std::ostream &operator<<(std::ostream &os, const ItemStats &stats);
};

inline std::ostream &operator<<(std::ostream &os, const ItemStats &stats)
{
    os << "ItemStats: accuracy = " << stats.accuracy << ", damage = " << stats.damage << ", attackSpeed = " << stats.attackSpeed << ", range = " << stats.range << ", armor = " << stats.armor << ", dodge = " << stats.dodge << std::endl;
    return os;
}
