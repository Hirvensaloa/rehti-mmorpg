#include <iostream>
#include <memory>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "Server.hpp"

uint16_t PORT = 9999;
int TICK_RATE = 32;
int TICK_TIME = 1000 / TICK_RATE;
static uint32_t id = 0;

Server::Server()
    : ioContextM(boost::asio::io_context()),
      acceptorM(boost::asio::ip::tcp::acceptor(ioContextM, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT))),
      messagesM(MessageQueue()),
      connectionsM(std::vector<std::shared_ptr<Connection>>()),
      workGuardM(boost::asio::make_work_guard(ioContextM)),
      gameWorldM(GameWorld())
{
    ioThreadM = std::thread([this]()
                            { ioContextM.run(); });
    acceptThreadM = std::thread([this]()
                                { acceptConnections(); });

    tickThreadM = std::thread([this]()
                              { ticker(); });

    std::cout
        << "Server started on port " << PORT << std::endl;
};

void Server::acceptConnections()
{
    while (true)
    {
        boost::asio::ip::tcp::socket socket(ioContextM);
        acceptorM.accept(socket);
        std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
                  << ":" << socket.remote_endpoint().port() << std::endl;
        std::shared_ptr<Connection> connection = std::make_shared<Connection>(
            Connection::owner::server, ioContextM, std::move(socket), messagesM);
        const bool connectSuccessful = connection->connectToClient(id++);

        if (connectSuccessful)
        {
            connectionsM.push_back(std::move(connection));
            gameWorldM.addPlayer("keissaaja" + std::to_string(connectionsM.back()->getID()), connectionsM.back()->getID(), Coordinates());

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
            if (msg.getHeader().id == MessageIds::Move)
            {
                auto id = msg.getConn()->getID();
                auto gamer = gameWorldM.getPlayer(id);
                int x, y;
                sscanf(msg.getBody().c_str(), "%d//%d", &x, &y);
                Coordinates target = Coordinates(x, y);
                gamer->setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), target, gamer));
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

void Server::ticker()
{
    while (true)
    {
        tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME));
    }
}

void Server::tick()
{
    for (PlayerCharacter p : gameWorldM.getPlayers())
    {
        p.getCurrentAction().act();
    }
}