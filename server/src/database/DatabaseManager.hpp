#pragma once

#include <pqxx/pqxx>

class PlayerCharacter;

struct PlayerTable
{
    int id;
    std::string username;
    std::string password;
    int position_x;
    int position_y;
    int hp;
};

/**
 * @brief Class for handling database queries
 */
class DatabaseManager
{
public:
    DatabaseManager();

    ~DatabaseManager() = default;

    /**
     * @brief Create connection to the database
     * @return 1 if successful, 0 if not
     */
    int createConnection();

    /**
     * @brief Fetches player basic data from the database (table player) by username.
     * Creates a new player with given username to the database if it does not yet exist
     * @param username
     * @return Player basic data as PlayerTable
     */
    PlayerTable loadPlayerDataFromDb(std::string username);

    /**
     * @brief Load player's inventory items from the database
     * @param playerId
     * @return Player's inventory items in a vector
     */
    std::vector<int> loadInventoryDataFromDb(int playerId);

    /**
     * @brief Load player's equipment items from the database
     * @param playerId
     * @return Player's equipment items in a vector
     */
    std::vector<int> loadEquipmentDataFromDb(int playerId);

    /**
     * @brief Load player's skill data from the database
     * @param playerId
     * @return Player's skill data in a vector
     */
    std::vector<std::pair<int, int>> loadSkillDataFromDb(int playerId);

    /**
     * @brief Save player's basic data, items and skills to the database
     * @param player
     * @return True if succesful, else false
     */
    bool savePlayerToDb(std::shared_ptr<PlayerCharacter> player);

private:
    std::unique_ptr<pqxx::connection> pConnectionM;
};
