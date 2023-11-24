#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <vector>

#include "database/DatabaseManager.hpp"
#include "world/GameWorld.hpp"
#include <network.h>

/**
 * @brief Main class for the server
 */
class Server
{
public:
    Server();

    /**
     * @brief Function to accept new connections from clients, runs on a seperate thread
     */
    void acceptConnections();

    /**
     * @brief Server's main loop for processing messages
     */
    void processMessages();

    /**
     * @brief Function to parse and act upon messages received from clients
     * @param msg
     */
    void handleMessage(const Message& msg);

private:
    /**
     * @brief Timer function that is used to start server ticks, runs on a separate thread
     */
    void ticker();

    /**
     * @brief Function that updates the gamestate on each server tick, called by ticker()
     */
    void tick();

    /**
     * @brief Sends the current gamestate to connected clients
     */
    void sendGameState();

    /**
     * @brief Initializes the gamestate and gameworld
     */
    void initGameState();

    /**
     * @brief Loads a player's data from the database and adds the player to the game
     * @param username
     * @param password
     * @param connection
     * @return True if player was loaded successfully, false otherwise
     */
    bool loadPlayer(std::string username, std::string password, const std::shared_ptr<Connection>& connection);

    /**
     * @brief Saves a player's data to the database
     * @param playerId
     */
    void savePlayer(int playerId);

    boost::asio::io_context ioContextM;

    boost::asio::ip::tcp::acceptor acceptorM;

    std::vector<std::shared_ptr<Connection>> connectionsM;

    MessageQueue messagesM;

    std::thread ioThreadM;

    std::thread acceptThreadM;

    std::thread tickThreadM;

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;

    GameWorld gameWorldM;

    DatabaseManager dbManagerM;
};