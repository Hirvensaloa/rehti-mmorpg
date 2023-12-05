#pragma once

#include <boost/asio/awaitable.hpp>

#include "RehtiReader.hpp"
#include "graphics.h"
#include <network.h>

class Client
{
public:
    Client(std::string ip, std::string port);

    /**
     * @brief Connects to the server
     *
     * @return true if connection was successful. False otherwise.
     */
    boost::asio::awaitable<bool> connect();

    /**
     * @brief Sends an attack message to the server
     *
     * @param targetId Id of the target entity
     */
    boost::asio::awaitable<void> attack(const int& targetId);

    /**
     * @brief Sends a move message to the server
     *
     * @param x coordinate to move to
     * @param y coordinate to move to
     */
    boost::asio::awaitable<void> move(const int& x, const int& y);

    /**
     * @brief Sends an object interaction message to the server.
     *
     * @param objectId the id of the object to interact with
     */
    boost::asio::awaitable<void> interactWithObject(const int& objectId);

    /**
     * @brief Send a UseItemMessage for item with given instance id
     * @param itemInstanceId
     */
    boost::asio::awaitable<void> useItem(const int itemInstanceId);

    /**
     * @brief Send a DropItemMessage for item with given instance id
     * @param itemInstanceId
     */
    boost::asio::awaitable<void> dropItem(const int itemInstanceId);

    /**
     * @brief Send a UnequipMessage for item with given instance id
     * @param itemInstanceId
     */
    boost::asio::awaitable<void> unequipItem(const int itemInstanceId);

    /**
     * @brief Send talk message to the server
     *
     * @param npcId Id of the npc to talk to
     */
    boost::asio::awaitable<void> talk(const int& npcId);

    /**
     * @brief Starts the client
     */
    void start();

    /**
     * @brief Prompts user for login information and sends a LoginMessage with that information. Currently only username used for identification
     * @return True if user inputs a valid username, false otherwise.
     */
    boost::asio::awaitable<bool> login();

    /**
     * @brief Processes messages from the server
     */
    void processMessages();

    /**
     * @brief Starts the graphics library
     */
    void startGraphics();

    /**
     * @brief Handles a mouse click hit on the graphics window
     * @param hit contains hit information. See Hit.
     */
    void handleMouseClick(const Hit& hit);

private:
    boost::asio::io_context ioContextM;
    boost::asio::ip::tcp::resolver resolverM;
    boost::asio::ip::tcp::resolver::results_type endpointsM;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;

    MessageQueue messagesM;
    std::unique_ptr<Connection> connectionM;

    std::thread graphicsThreadM;
    std::thread connectionThreadM;
    std::thread ioThreadM;

    RehtiGraphics* pGraphLibM;
    std::condition_variable graphLibReadyM; ///< GraphLib ready lock
    bool graphLibReadyFlagM = false;        ///< GraphLib ready flag
    std::mutex graphLibMutexM;

    std::condition_variable loggedInM; ///< Login lock
    bool loggedInFlagM = false;        ///< Login flag
    std::mutex loginMutexM;

    Hit lastHitM; ///< Last detected hit from a mouse click

    AssetCache& assetCacheM = AssetCache::getInstance();
};