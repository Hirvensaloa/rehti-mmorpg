#include <iostream>
#include <memory>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "Server.hpp"

uint16_t PORT = 9999;

static int testId = 1;

Server::Server()
    : ioContextM(boost::asio::io_context()),
      acceptorM(boost::asio::ip::tcp::acceptor(ioContextM, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT))),
      messagesM(MessageQueue()),
      connectionsM(std::vector<std::unique_ptr<Connection>>()),
      workGuardM(boost::asio::make_work_guard(ioContextM)),
      gameWorldM(GameWorld())
{
    ioThreadM = std::thread([this]()
                            { ioContextM.run(); });
    acceptThreadM = std::thread([this]()
                                { acceptConnections(); });

    std::cout << "Server started on port " << PORT << std::endl;
};

void Server::acceptConnections()
{
    while (true)
    {
        boost::asio::ip::tcp::socket socket(ioContextM);
        acceptorM.accept(socket);
        std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
                  << ":" << socket.remote_endpoint().port() << std::endl;

        std::unique_ptr<Connection> connection = std::make_unique<Connection>(
            Connection::owner::server, ioContextM, std::move(socket), messagesM);
        const bool connectSuccessful = connection->connectToClient();

        if (connectSuccessful)
        {
            connectionsM.push_back(std::move(connection));
            gameWorldM.addPlayer("keissaaja" + std::to_string(testId), testId, Coordinates());
            testId++;

            boost::asio::co_spawn(ioContextM, connectionsM.back()->listenForMessages(), boost::asio::detached);
            boost::asio::co_spawn(ioContextM, connectionsM.back()->send(MessageIds::Test, "Hello gamer, your name is " + gameWorldM.getPlayers().back().getName() + "!\0"), boost::asio::detached);
        }
        else
        {
            std::cout << "Failed to connect to client!" << std::endl;
        }
    }
}

void Server::processMessages()
{
    while (true)
    {
        if (!messagesM.empty())
        {
            Message msg = messagesM.pop_front();
            std::cout << msg.getBody() << std::endl;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}