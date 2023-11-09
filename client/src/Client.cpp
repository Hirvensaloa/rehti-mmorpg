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
  graphicsThreadM = std::thread([this]()
                                { startGraphics(); });
  connectionThreadM = std::thread([this]()
                                  { boost::asio::co_spawn(ioContextM, connect(), boost::asio::detached); });

  ioThreadM = std::thread([this]()
                          { ioContextM.run(); });
};

void Client::start()
{
  // Wait for graphics library to be ready
  if (!graphLibReadyFlagM)
  {
    std::unique_lock ul(graphLibMutexM);
    graphLibReadyM.wait(ul);
  }

  std::cout << "Starting to process messages..." << std::endl;
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

        const auto obj = gameObjectsObjDataM["ukko"];
        pGraphLibM->addGameObject(gameStateMsg.currentPlayer.entityId, obj.vertices, obj.indices, textureDataM["ukkotextuuri1.png"], {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
        pGraphLibM->forcePlayerMove(gameStateMsg.currentPlayer.entityId, {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
        std::cout << "player"
                  << " " << gameStateMsg.currentPlayer.entityId << " " << gameStateMsg.currentPlayer.x << " " << gameStateMsg.currentPlayer.y << " " << gameStateMsg.currentPlayer.z << std::endl;

        for (const auto &entity : gameStateMsg.entities)
        {
          // Ignore the player itself
          if (entity.entityId == gameStateMsg.currentPlayer.entityId)
          {
            continue;
          }

          std::cout << "entity"
                    << " " << entity.entityId << " " << entity.x << " " << entity.y << " " << entity.z << std::endl;
          const auto obj = gameObjectsObjDataM["orc1"];
          pGraphLibM->addGameObject(entity.entityId, obj.vertices, obj.indices, textureDataM["sand.png"], {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
          pGraphLibM->forceGameObjectMove(entity.entityId, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
        }
        for (const auto &object : gameStateMsg.objects)
        {
          const std::string idStr = std::to_string(object.id);
          if (gameObjectsObjDataM.contains(idStr))
          {
            std::cout << "object"
                      << " " << object.id << " " << object.x << " " << object.y << " " << object.z << std::endl;
            const auto obj = gameObjectsObjDataM[idStr];
            if (object.id == 1)
            {
              pGraphLibM->addGameObject(std::stoi(object.instanceId), obj.vertices, obj.indices, textureDataM["treetexture.png"], {object.x, Config.HEIGHT_MAP_SCALE * object.z, object.y});
            }
            else
            {
              pGraphLibM->addGameObject(std::stoi(object.instanceId), obj.vertices, obj.indices, textureDataM["housetexture.png"], {object.x, Config.HEIGHT_MAP_SCALE * object.z, object.y});
            }
          }
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
			const Hit& hit = this->lastHitM;
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
  pGraphLibM = new RehtiGraphics();

  // Create map bounding box
  std::vector<std::vector<int>> heightMatrix;
  std::vector<std::vector<std::string>> areaMatrix;
  loadHeightMap(heightMatrix, Config.GENERATED_HEIGHT_MAP_PATH);
  loadAreaMap(areaMatrix, Config.GENERATED_AREA_MAP_PATH);
  pGraphLibM->addMapBoundingBox(MapAABBData{heightMatrix, areaMatrix, Config.AREA_WIDTH, Config.HEIGHT_MAP_SCALE, Config.TILE_SIDE_SCALE, Config.TILE_SIDE_UNIT});

  // Load object obj files into memory
  const auto gameObjectsData = loadGameObjectObjs();
  for (const auto &obj : gameObjectsData)
  {
    gameObjectsObjDataM[obj.first] = createGameObjectGraphicData(obj.second.vertices, obj.second.faces);
  }

  // Load texture files into memory
  const auto textures = loadTextures();
  for (const auto &texture : textures)
  {
    textureDataM[texture.first] = stbImageDataToImageData(texture.second);
    std::cout << "Loaded texture " << texture.first << std::endl;
  }

  // Load map area obj files into memory
  std::vector<std::vector<aiVector3D>> areaVertexList;
  std::vector<std::vector<aiFace>> areaFaceList;
  loadAreaMapObjs(areaMatrix, areaVertexList, areaFaceList);
  std::array<ImageData, 6> areaTextures;

  for (size_t i = 0; i < areaTextures.size(); i++)
  {
    areaTextures[i] = TestValues::GetTestTexture();
  }
  std::vector<std::vector<Vertex>> areaVertices = aiVector3DMatrixToVertexVector(areaVertexList);
  std::vector<std::vector<uint32_t>> areaIndices = aiFaceMatrixToVector(areaFaceList);
  for (int i = 0; i < areaVertices.size(); i++)
  {
    pGraphLibM->addArea(areaVertices[i], areaIndices[i], areaTextures);
  }

  // Add action callbacks
  pGraphLibM->addMouseClickCallback([this](const Hit &hit)
                                    { handleMouseClick(hit); });

  std::cout << "Graphics library ready" << std::endl;

  graphLibReadyFlagM = true;
  graphLibReadyM.notify_one();
  pGraphLibM->demo();
}