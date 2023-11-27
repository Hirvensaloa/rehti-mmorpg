#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <memory>

#include "Client.hpp"
#include "RehtiReader.hpp"
#include "Utils.hpp"

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
        msg.objectId = std::to_string(objectId);
        co_await connectionM->send(MessageApi::createObjectInteract(msg));
    }
}

boost::asio::awaitable<void> Client::useItem(const int itemInstanceId)
{
    if (connectionM->isConnected())
    {
        UseItemMessage msg;
        msg.itemId = itemInstanceId;
        co_await connectionM->send(MessageApi::createUseItem(msg));
    }
    co_return;
}

boost::asio::awaitable<void> Client::dropItem(const int itemInstanceId)
{
    if (connectionM->isConnected())
    {
        DropItemMessage msg;
        msg.itemId = itemInstanceId;
        co_await connectionM->send(MessageApi::createDropItem(msg));
    }
    co_return;
}

boost::asio::awaitable<void> Client::unequipItem(const int itemInstanceId)
{
    if (connectionM->isConnected())
    {
        UnequipMessage msg;
        msg.itemId = itemInstanceId;
        co_await connectionM->send(MessageApi::createUnequip(msg));
    }
    co_return;
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

            const MessageId msgId = static_cast<MessageId>(msg.getHeader().id);

            try
            {
                if (msgId == MessageId::GameState)
                {
                    const GameStateMessage& gameStateMsg = MessageApi::parseGameState(msg.getBody());

                    const auto charAsset = assetCacheM.getCharacterAssetDataById(0); // TODO: Currently player id refers to the player id in db. We should also have a way to indicate if the entity is a player or not. Currently the problem is that the player id might interfere with entity ids.
                    pGraphLibM->addCharacterObject(gameStateMsg.currentPlayer.id, charAsset.vertices, charAsset.indices, charAsset.texture, charAsset.animations, charAsset.bones, charAsset.boneTransformations, {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
                    pGraphLibM->forcePlayerMove(gameStateMsg.currentPlayer.id, {gameStateMsg.currentPlayer.x, Config.HEIGHT_MAP_SCALE * gameStateMsg.currentPlayer.z, gameStateMsg.currentPlayer.y});
                    pGraphLibM->playAnimation(gameStateMsg.currentPlayer.id, actionToAnimationConfig(gameStateMsg.currentPlayer.currentAction));
                    pGraphLibM->getGui()->setInventory(gameStateMsg.currentPlayer.inventory);

                    pGraphLibM->getGui()->setSkills(gameStateMsg.currentPlayer.skills);

                    for (const auto& entity : gameStateMsg.entities)
                    {
                        // Do not draw the current player twice
                        if (entity.instanceId == gameStateMsg.currentPlayer.id)
                        {
                            pGraphLibM->getGui()->setEquipment(entity.equipment);
                            continue;
                        }

                        const auto charAsset = assetCacheM.getCharacterAssetDataById(entity.id);
                        pGraphLibM->addCharacterObject(entity.instanceId, charAsset.vertices, charAsset.indices, charAsset.texture, charAsset.animations, charAsset.bones, charAsset.boneTransformations, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                        pGraphLibM->forceCharacterMove(entity.instanceId, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                        pGraphLibM->playAnimation(entity.instanceId, actionToAnimationConfig(entity.currentAction));
                    }
                    for (const auto& object : gameStateMsg.objects)
                    {
                        const auto objAsset = assetCacheM.getObjectAssetDataById(object.id);
                        // Convert rotation 0, 1, 2, 3 to 0, pi/2, pi, 3pi/2
                        const float rotation = object.rotation * (M_PI / 2);
                        pGraphLibM->addGameObject(std::stoi(object.instanceId), objAsset.vertices, objAsset.indices, objAsset.texture, {object.x, Config.HEIGHT_MAP_SCALE * object.z, object.y}, rotation);
                    }
                }
                else if (msgId == MessageId::Informative)
                {
                    const InformativeMessage& informativeMsg = MessageApi::parseInformative(msg.getBody());
                    std::cout << "Message from server: " << informativeMsg.message << std::endl;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to parse message from server: " << e.what() << '\n';
            }
        };
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
            while (k < Config.MAX_MAP_TEXTURES)
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

    pGraphLibM->getGui()->addInventoryItemClickCallback([this](const int itemInstanceId)
                                                        { boost::asio::co_spawn(ioContextM, useItem(itemInstanceId), boost::asio::detached); });

    pGraphLibM->getGui()->addDropItemCallback([this](const int itemInstanceId)
                                              { boost::asio::co_spawn(ioContextM, dropItem(itemInstanceId), boost::asio::detached); });

    pGraphLibM->getGui()->addEquipmentItemClickCallback([this](const int itemInstanceId)
                                                        { boost::asio::co_spawn(ioContextM, unequipItem(itemInstanceId), boost::asio::detached); });

    std::cout << "Graphics library ready" << std::endl;

    graphLibReadyFlagM = true;
    graphLibReadyM.notify_one();
    pGraphLibM->demo();
}