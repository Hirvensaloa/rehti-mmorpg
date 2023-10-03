#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <vector>

#include "GameWorld.hpp"
#include <network.h>

class Server
{
public:
    Server();

    void acceptConnections();

    void processMessages();

private:
    boost::asio::io_context ioContextM;
    boost::asio::ip::tcp::acceptor acceptorM;
    std::vector<std::unique_ptr<Connection>> connectionsM;
    MessageQueue messagesM;
    std::thread ioThreadM;
    std::thread acceptThreadM;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;
    GameWorld gameWorldM;
};