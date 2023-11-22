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

boost::asio::awaitable<bool> Client::login()
{
    if (connectionM->isConnected())
    {
        std::string usr = "";
        std::string pwd = "";
        std::cout << "------------------ Login (or signup) to Rehti ------------------" << std::endl
                  << "Username: ";
        std::cin >> usr;
        std::cout << "Password: ";
        std::cin >> pwd;
        if (usr != "" && pwd != "")
        {
            LoginMessage msg;
            msg.username = usr;
            msg.password = pwd;
            co_await connectionM->send(MessageApi::createLogin(msg));
            boost::asio::co_spawn(ioContextM, connectionM->listenForMessages(), boost::asio::detached);

            co_return true;
        }
        else
        {
            std::cout << "Invalid username" << std::endl;
            co_return false;
        }
    }
}

boost::asio::awaitable<bool> Client::connect()
{
    try
    {
        std::cout << "Connecting to server..." << std::endl;
        const bool ret = co_await connectionM->connectToServer(endpointsM);

        if (ret)
        {
            co_await login();
        }

        // Set the logged in flag to true anyway so that the graphics backend can be started regardless of server being online or not. TODO: Remove when the login happens on UI, not on console.
        loggedInFlagM = true;
        loggedInM.notify_one();

        co_return ret;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        co_return false;
    }
}

boost::asio::awaitable<void> Client::attack(const int& targetId)
{
    if (connectionM->isConnected())
    {
        AttackMessage msg;
        msg.targetId = targetId;
        co_await connectionM->send(MessageApi::createAttack(msg));
    }
}

boost::asio::awaitable<void> Client::move(const int& x, const int& y)
{
    if (connectionM->isConnected())
    {
        MoveMessage msg;
        msg.x = x;
        msg.y = y;
        co_await connectionM->send(MessageApi::createMove(msg));
    }
}

boost::asio::awaitable<void> Client::interactWithObject(const int& objectId)
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
                const GameStateMessage& gameStateMsg = MessageApi::parseGameState(msg.getBody());

                const auto objAsset = assetCacheM.getCharacterAssetDataById("ukko");
                pGraphLibM->addGameObject(gameStateMsg.currentPlayer.entityId, objAsset.vertices, objAsset.indices, objAsset.texture, {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
                pGraphLibM->forcePlayerMove(gameStateMsg.currentPlayer.entityId, {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
                std::cout << "player"
                          << " " << gameStateMsg.currentPlayer.entityId << " " << gameStateMsg.currentPlayer.x << " " << gameStateMsg.currentPlayer.y << " " << gameStateMsg.currentPlayer.z << std::endl;

                for (const auto& entity : gameStateMsg.entities)
                {
                    // Do not draw the current player twice
                    if (entity.entityId == gameStateMsg.currentPlayer.entityId)
                    {
                        continue;
                    }

                    std::cout << "entity"
                              << " " << entity.entityId << " " << entity.x << " " << entity.y << " " << entity.z << std::endl;
                    const auto objAsset = assetCacheM.getCharacterAssetDataById("goblin");
                    pGraphLibM->addGameObject(entity.entityId, objAsset.vertices, objAsset.indices, objAsset.texture, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                    pGraphLibM->forceGameObjectMove(entity.entityId, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                }
                for (const auto& object : gameStateMsg.objects)
                {
                    std::cout << "object"
                              << " " << object.id << " " << object.x << " " << object.y << " " << object.z << std::endl;
                    const auto objAsset = assetCacheM.getObjectAssetDataById(object.id);
                    pGraphLibM->addGameObject(std::stoi(object.instanceId), objAsset.vertices, objAsset.indices, objAsset.texture, {object.x, Config.HEIGHT_MAP_SCALE * object.z, object.y});
                }
            }
            else if (msgId == MessageId::Informative)
            {
                const InformativeMessage& informativeMsg = MessageApi::parseInformative(msg.getBody());
                std::cout << "Message from server: " << informativeMsg.message << std::endl;
            }
        }
    }
}

void Client::handleMouseClick(const Hit& hit)
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
    // Wait for login to be successful
    if (!loggedInFlagM)
    {
        std::unique_lock ul(loginMutexM);
        loggedInM.wait(ul);
    }

    pGraphLibM = new RehtiGraphics();

    // Create map bounding box
    std::vector<std::vector<int>> heightMatrix = fetchHeightMatrix();
    std::vector<std::vector<std::string>> areaMatrix = fetchAreaMatrix();

    // Load assets to memory
    assetCacheM.loadAssets();

    // Add areas to the graphics backend
    pGraphLibM->addMapBoundingBox(MapAABBData{heightMatrix, areaMatrix, Config.AREA_WIDTH, Config.HEIGHT_MAP_SCALE, Config.TILE_SIDE_SCALE, Config.TILE_SIDE_UNIT});
    for (int i = 0; i < areaMatrix.size(); i++)
    {
        for (int j = 0; j < areaMatrix[i].size(); j++)
        {
            const auto& areaAssetData = assetCacheM.getAreaAssetData()[areaMatrix[i][j] + std::to_string(i) + std::to_string(j)];
            std::array<ImageData, 6> textures = {areaAssetData.blendMap, ImageData(), ImageData(), ImageData(), ImageData(), ImageData()};

            // Loop through the area textures and use default texture for leftovers
            int k = 0;
            while (k <= Config.MAX_MAP_TEXTURES)
            {
                if (areaAssetData.textures.size() > k && areaAssetData.textures[k].width != 0 && areaAssetData.textures[k].height != 0)
                {
                    textures[k + 1] = areaAssetData.textures[k];
                }
                else
                {
                    textures[k + 1] = assetCacheM.getDefaultTexture();
                }
                k++;
            };

            pGraphLibM->addArea(areaAssetData.vertices, areaAssetData.indices, textures);
        }
    }

    // Add action callbacks
    pGraphLibM->addMouseClickCallback([this](const Hit& hit)
                                      { handleMouseClick(hit); });

    std::cout << "Graphics library ready" << std::endl;

    graphLibReadyFlagM = true;
    graphLibReadyM.notify_one();
    pGraphLibM->demo();
}