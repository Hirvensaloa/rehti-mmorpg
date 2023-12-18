#include "DatabaseManager.hpp"
#include "../entity/PlayerCharacter.hpp"
#include "bcrypt.h"

#include <iostream>

const char* DB_NAME = std::getenv("POSTGRES_DB");
const char* DB_USER = std::getenv("POSTGRES_USER");
const char* DB_PASSWORD = std::getenv("POSTGRES_PASSWORD");
const char* DB_HOST = std::getenv("DB_HOST");
const char* DB_PORT = std::getenv("DB_PORT");

DatabaseManager::DatabaseManager() : pConnectionM{nullptr}
{
    createConnection();
}

int DatabaseManager::createConnection()
{
    try
    {
        // Connect to the db
        std::stringstream connectionParams;
        connectionParams << "dbname=" << DB_NAME << " user=" << DB_USER << " password=" << DB_PASSWORD << " host=" << DB_HOST << " port=" << DB_PORT
                         << " connect_timeout=10";
        pConnectionM = std::make_unique<pqxx::connection>(connectionParams.str());
        std::cout << "Connected to " << pConnectionM->dbname() << '\n';
    }
    catch (std::exception const& e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 0;
    }
    return 1;
}

PlayerTable DatabaseManager::loadPlayerDataFromDb(std::string username, std::string password, Coordinates spawnCoordinate)
{
    if (pConnectionM == nullptr || !pConnectionM->is_open())
    {
        createConnection();
    }

    pqxx::work txn{*pConnectionM};
    const std::string sanitizedUsername = txn.esc(username);
    pqxx::result r{txn.exec("SELECT id, username, password, position_x, position_y, hp FROM player WHERE username='" + sanitizedUsername + "'")};
    if (!r.empty())
    {
        txn.commit();
        PlayerTable res;
        res.id = r[0][0].as<int>();
        res.username = std::string(r[0][1].c_str());
        res.password = std::string(r[0][2].c_str());
        res.position_x = r[0][3].as<int>();
        res.position_y = r[0][4].as<int>();
        res.hp = r[0][5].as<int>();

        if (bcrypt::validatePassword(password, res.password))
        {
            return res;
        }
        else
        {
            throw std::runtime_error("Invalid password");
        }
        return res;
    }
    else
    {
        const std::string sanitizedX = txn.esc(std::to_string(spawnCoordinate.x));
        const std::string sanitizedY = txn.esc(std::to_string(spawnCoordinate.y));
        std::string hash = bcrypt::generateHash(password);
        pqxx::result r{txn.exec("INSERT INTO player VALUES (DEFAULT,'" + sanitizedUsername + "','" + hash + "'," + sanitizedX + "," + sanitizedY + ",100) RETURNING id, username, password, position_x, position_y, hp")};
        PlayerTable res;
        res.id = r[0][0].as<int>();
        res.username = std::string(r[0][1].c_str());
        res.password = std::string(r[0][2].c_str());
        res.position_x = r[0][3].as<int>();
        res.position_y = r[0][4].as<int>();
        res.hp = r[0][5].as<int>();
        txn.commit();
        return res;
    }
}

std::vector<int> DatabaseManager::loadInventoryDataFromDb(int playerId)
{
    if (pConnectionM == nullptr || !pConnectionM->is_open())
    {
        createConnection();
    }

    pqxx::work txn{*pConnectionM};
    pqxx::result r{txn.exec("SELECT id, amount FROM item WHERE player_id=" + std::to_string(playerId) + " AND is_equipped=false")};
    txn.commit();
    std::vector<int> itemIds;
    for (auto row : r)
    {
        for (int i = 0; i < row[1].as<int>(); i++)
        {
            itemIds.push_back(row[0].as<int>());
        }
    }
    return itemIds;
}

std::vector<int> DatabaseManager::loadEquipmentDataFromDb(int playerId)
{
    if (pConnectionM == nullptr || !pConnectionM->is_open())
    {
        createConnection();
    }

    pqxx::work txn{*pConnectionM};
    pqxx::result r{txn.exec("SELECT id FROM item WHERE player_id=" + std::to_string(playerId) + " AND is_equipped=true")};
    txn.commit();
    std::vector<int> itemIds;
    for (auto row : r)
    {
        itemIds.push_back(row[0].as<int>());
    }
    return itemIds;
}

std::vector<std::pair<int, int>> DatabaseManager::loadSkillDataFromDb(int playerId)
{
    if (pConnectionM == nullptr || !pConnectionM->is_open())
    {
        createConnection();
    }

    pqxx::work txn{*pConnectionM};
    pqxx::result r{txn.exec("SELECT id, exp FROM skill WHERE player_id=" + std::to_string(playerId))};
    txn.commit();
    std::vector<std::pair<int, int>> skillData;
    for (auto row : r)
    {
        skillData.push_back(std::make_pair(row[0].as<int>(), row[1].as<int>()));
    }
    return skillData;
}

bool DatabaseManager::savePlayerToDb(std::shared_ptr<PlayerCharacter> player)
{
    if (pConnectionM == nullptr || !pConnectionM->is_open())
    {
        createConnection();
    }

    pqxx::nontransaction txn{*pConnectionM};
    auto loc = player->getLocation();
    auto id = player->getId();
    txn.exec("begin;");
    pqxx::result r{txn.exec("UPDATE player SET (position_x, position_y, hp) = (" + std::to_string(loc.x) + "," + std::to_string(loc.y) + "," + std::to_string(player->getHp()) + ") WHERE id=" + std::to_string(id))};

    r = txn.exec("DELETE FROM item WHERE player_id=" + std::to_string(id) + " AND is_equipped=false");
    for (auto item : player->getInventory().getItems())
    {
        r = txn.exec("INSERT INTO item VALUES(" + std::to_string(item->getId()) + "," + std::to_string(id) + ",1,false) ON CONFLICT (id, player_id, is_equipped) DO UPDATE SET amount = item.amount +1");
    }

    txn.exec("DELETE FROM item WHERE player_id=" + std::to_string(id) + " AND is_equipped=true");
    for (auto& item : player->getEquipment().getAllEquipment())
    {
        r = txn.exec("INSERT INTO item VALUES(" + std::to_string(item->getId()) + "," + std::to_string(id) + ",1,true) ON CONFLICT (id, player_id, is_equipped) DO NOTHING");
    }

    for (auto skill : player->getSkillSet().getSkills())
    {
        r = txn.exec("INSERT INTO skill VALUES(" + std::to_string(skill.first) + "," + std::to_string(id) + "," + std::to_string(skill.second.xp) + " ) ON CONFLICT (id, player_id) DO UPDATE SET exp = EXCLUDED.exp ");
    }
    txn.exec("commit;");
    txn.commit();

    return true;
}
