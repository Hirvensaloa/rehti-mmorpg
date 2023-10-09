#include <iostream>
#include <memory>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "Server.hpp"

uint16_t PORT = 9999;
int TICK_RATE = 1;
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

    initGameState();

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
        // Wait for messages to arrive
        messagesM.wait();

        while (!messagesM.empty())
        {
            Message msg = messagesM.pop_front();
            handleMessage(msg);
        }
    }
}

void Server::handleMessage(const Message &msg)
{
    try
    {
        const auto body = msg.getBody();
        const auto msgId = msg.getHeader().id;
        const auto connId = msg.getConn()->getID();

        switch (msgId)
        {
        case MessageId::Move:
        {
            std::cout << connId << " | Received move message | " << body << std::endl;
            const MoveMessage moveMsg = MessageApi::parseMove(body);
            const Coordinates target = Coordinates(moveMsg.x, moveMsg.y);
            auto gamer = gameWorldM.getPlayer(connId);
            gamer->setAction(std::make_shared<MoveAction>(std::chrono::system_clock::now(), target, gamer));
            break;
        }
        case MessageId::Attack:
        {
            std::cout << "Attack message received. TODO: implement" << std::endl;
            break;
        }
        default:
            // Unknown header id, ignore
            std::cout << "Unknown header id: " << msgId << std::endl;
            break;
        }
    }
    catch (const std::exception &e)
    {
        // Parsing failed, ignore error and continue
        std::cout << "Error parsing message: " << e.what() << std::endl;
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
    if (connectionsM.empty())
    {
        return;
    }

    gameWorldM.updateGameWorld();
    std::thread(&Server::sendGameState, this).detach();
}

void Server::sendGameState()
{
    GameStateMessage msg;
    std::vector<GameStateEntity> entityVector;
    for (auto &npc : gameWorldM.getNpcs())
    {
        GameStateEntity entity;
        const Coordinates location = npc.getLocation();
        entity.entityId = npc.getId();
        entity.name = npc.getName();
        entity.x = location.x;
        entity.y = location.y;
        entity.z = location.z;
        entityVector.push_back(entity);
    }

    for (auto &player : gameWorldM.getPlayers())
    {
        GameStateEntity entity;
        const Coordinates location = player.getLocation();
        entity.entityId = player.getId();
        entity.name = player.getName();
        entity.x = location.x;
        entity.y = location.y;
        entity.z = location.z;
        entityVector.push_back(entity);
    }
    msg.entities = entityVector;

    for (auto &conn : connectionsM)
    {
        if (conn->isConnected())
        {
            boost::asio::co_spawn(ioContextM, conn->send(MessageApi::createGameState(msg)), boost::asio::detached);
        }
    }
}

void Server::initGameState()
{
    gameWorldM.initWorld();
}