#pragma once

#include <network.h>

class Server
{
public:
    Server();

    void acceptConnection();

    void processMessages();

private:
    boost::asio::io_context ioContextM;
    boost::asio::ip::tcp::acceptor acceptorM;
    std::vector<std::unique_ptr<Connection>> connectionsM;
    MessageQueue<Message> messagesM;
    std::thread ioThreadM;
    std::thread acceptThreadM;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;
};