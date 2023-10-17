#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <vector>

#include "world/GameWorld.hpp"
#include <network.h>

class Server
{
public:
    Server();

    void acceptConnections();

    void processMessages();

    void handleMessage(const Message &msg);

private:
    void ticker();

    void tick();

    void sendGameState();

    void initGameState();

    boost::asio::io_context ioContextM;

    boost::asio::ip::tcp::acceptor acceptorM;

    std::vector<std::shared_ptr<Connection>> connectionsM;

    MessageQueue messagesM;

    std::thread ioThreadM;

    std::thread acceptThreadM;

    std::thread tickThreadM;

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;

    GameWorld gameWorldM;
};