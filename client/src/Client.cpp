#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <memory>

#include "Client.hpp"
#include "RehtiReader.hpp"

Client::Client(std::string ip, std::string port)
    : ioContextM(boost::asio::io_context()),
      resolverM(boost::asio::ip::tcp::resolver(ioContextM)),
      endpointsM(resolverM.resolve(ip, port)),
      workGuardM(boost::asio::make_work_guard(ioContextM)),
      messagesM(MessageQueue()),
      connectionM(std::make_unique<Connection>(
          Connection::owner::client, ioContextM, std::move(boost::asio::ip::tcp::socket(ioContextM)), messagesM))
{
  this->startGraphics();
  connectionThreadM = std::thread([this]()
                                  { boost::asio::co_spawn(ioContextM, connect(), boost::asio::detached); });

  ioThreadM = std::thread([this]()
                          { ioContextM.run(); });
};

void Client::start()
{
  std::cout << "Client started" << std::endl;
  std::thread([this]()
              { test(); })
      .detach();
  processMessages();
}

boost::asio::awaitable<bool> Client::connect()
{
  try
  {
    std::cout << "Connecting to server..." << std::endl;
    const bool ret = co_await connectionM->connectToServer(endpointsM);
    std::cout << ret << std::endl;

    if (ret)
    {
      boost::asio::co_spawn(ioContextM, connectionM->listenForMessages(), boost::asio::detached);
    }

    co_return ret;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    co_return false;
  }
}

boost::asio::awaitable<void> Client::randomWalk()
{
  if (connectionM->isConnected())
  {
    AttackMessage msg;
    msg.targetId = 123;
    co_await connectionM->send(MessageApi::createAttack(msg));
  }
}

boost::asio::awaitable<void> Client::attack(const int &targetId)
{
  if (connectionM->isConnected())
  {
    AttackMessage msg;
    msg.targetId = targetId;
    co_await connectionM->send(MessageApi::createAttack(msg));
  }
}

boost::asio::awaitable<void> Client::move(const int &x, const int &y)
{
  if (connectionM->isConnected())
  {
    MoveMessage msg;
    msg.x = x;
    msg.y = y;
    co_await connectionM->send(MessageApi::createMove(msg));
  }
}

boost::asio::awaitable<void> Client::interactWithObject(const int &objectId)
{
  if (connectionM->isConnected())
  {
    ObjectInteractMessage msg;
    msg.objectId = objectId;
    co_await connectionM->send(MessageApi::createObjectInteract(msg));
  }
}

void Client::test()
{
  while (true)
  {
    boost::asio::co_spawn(
        ioContextM, [this]() -> boost::asio::awaitable<void>
        { co_await randomWalk(); },
        boost::asio::detached);
    std::this_thread::sleep_for(std::chrono::milliseconds(115000));
  }
}

void Client::processMessages()
{
  while (true)
  {
    // Wait for message to arrive
    messagesM.wait();

    while (!messagesM.empty())
    {
      Message msg = messagesM.pop_front();

      const int msgId = msg.getHeader().id;

      if (msgId == MessageId::GameState)
      {
        const GameStateMessage &gameStateMsg = MessageApi::parseGameState(msg.getBody());
        for (const auto &entity : gameStateMsg.entities)
        {
          // TODO: Draw each entity here
        }
        for (const auto &object : gameStateMsg.objects)
        {
          // TODO: Draw each object here
        }
      }
      else
      {
        std::cout << "Unknown message id: " << msgId << std::endl;
      }
    }
  }
}

void Client::handleMouseClick(const Hit &hit)
{
  lastHitM = hit;
  std::cout << "Mouse click hit" << std::endl;
  boost::asio::co_spawn(
      ioContextM, [this]() -> boost::asio::awaitable<void>
      {   
        const Hit &hit = this->lastHitM;
        switch (hit.objectType)
        {
        case ObjectType::AREA:
          std::cout << "Hit tile on " << hit.hitPoint.x << " " << hit.hitPoint.z << std::endl;
          co_await move(hit.hitPoint.x, hit.hitPoint.z);
          break;
        case ObjectType::CHARACTER:
          std::cout << "Character" << std::endl;
          co_await attack(hit.id);
          break;
        case ObjectType::GAMEOBJECT:
          co_await interactWithObject(hit.id);
          std::cout << "Game object" << std::endl;
          break;
        case ObjectType::UNDEFINED:
          std::cout << "Miss" << std::endl;
          break;
        default:
          std::cout << "Unknown ObjectType" << std::endl;
          break;
        } },
      boost::asio::detached);
}

void Client::startGraphics()
{
  std::thread(
      [this]()
      {
        // Create map bounding box
        std::vector<std::vector<int>> heightMatrix;
        std::vector<std::vector<std::string>> areaMatrix;
        loadHeightMap(heightMatrix, Config.GENERATED_HEIGHT_MAP_PATH);
        loadAreaMap(areaMatrix, Config.GENERATED_AREA_MAP_PATH);
        graphLib.addMapBoundingBox(MapAABBData{heightMatrix, areaMatrix, Config.AREA_WIDTH, Config.HEIGHT_MAP_SCALE, Config.TILE_SIDE_SCALE, Config.TILE_SIDE_UNIT});

        // Load map area obj files into memory
        std::vector<std::vector<aiVector3D>> areaVertexList;
        std::vector<std::vector<aiFace>> areaFaceList;
        loadAreaMapObjs(areaMatrix, areaVertexList, areaFaceList);
        for (int i = 0; i < areaVertexList.size(); i++)
        {
          // TODO: load area objs using RehtiGraphics interface. @Otso voit käyttää tätä
        }

        // Add action callbacks
        graphLib.addMouseClickCallback([this](const Hit &hit)
                                       { handleMouseClick(hit); });

        graphLib.demo();
      })
      .detach();
}