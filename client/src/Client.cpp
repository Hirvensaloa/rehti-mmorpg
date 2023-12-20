#include "GLFW/glfw3.h"
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
          Connection::owner::client, ioContextM, std::move(boost::asio::ip::tcp::socket(ioContextM)), messagesM)),
      audioLibM{}
{
    graphicsThreadM = std::thread([this]()
                                  { startGraphics(); });

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
    connectionThreadM = std::thread([this]()
                                    { boost::asio::co_spawn(ioContextM, connect(), boost::asio::detached); });

    std::thread messageThread([this]()
                              { processMessages(); });
    // Cleanup after graphics thread has exited
    graphicsThreadM.join();
    connectionM->disconnect();
    ioContextM.stop();
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
            audioLibM.playMusic();
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

boost::asio::awaitable<void> Client::talk(const int& npcId)
{
    if (connectionM->isConnected())
    {
        TalkMessage msg;
        msg.npcId = npcId;
        co_await connectionM->send(MessageApi::createTalk(msg));
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

boost::asio::awaitable<void> Client::pickUpItem(const int& itemId, const int& x, const int& y)
{
    if (connectionM->isConnected())
    {

        PickUpItemMessage msg;
        msg.itemId = (-itemId);
        msg.x = x;
        msg.y = y;
        co_await connectionM->send(MessageApi::createPickUpItem(msg));
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

                    std::shared_ptr<RehtiGui> pGui = pGraphLibM->getGui();

                    const auto& currentPlayer = gameStateMsg.currentPlayer;
                    const auto& prevCurrentPlayer = prevGameStateMsgM.currentPlayer;
                    if (currentPlayer != prevCurrentPlayer)
                    {
                        const auto charAsset = assetCacheM.getCharacterAssetDataById(currentPlayer.id);
                        pGraphLibM->addCharacterObject(currentPlayer.instanceId, charAsset.vertices, charAsset.indices, charAsset.texture, charAsset.animations, charAsset.bones, charAsset.boneTransformations, {currentPlayer.x, Config.HEIGHT_MAP_SCALE * currentPlayer.z, currentPlayer.y}, 0.f, true);
                        pGui->setInventory(currentPlayer.inventory);
                        pGui->setSkills(currentPlayer.skills);
                        pGui->setHp(currentPlayer.hp);
                    }
                    else
                    {
                        if (!currentPlayer.hasSameActionAs(prevCurrentPlayer))
                        {
                            // Move action is special case as we need to animate and move the entity at the same time
                            if (currentPlayer.currentAction.id == ActionType::Move)
                            {
                                const auto& coords = currentPlayer.currentAction.targetCoordinate;
                                pGraphLibM->movePlayer(currentPlayer.instanceId, {coords.x, Config.HEIGHT_MAP_SCALE * coords.z, coords.y}, currentPlayer.currentAction.durationMs / 1000.0f);
                            }
                            else if (currentPlayer.currentAction.id == ActionType::Respawn)
                            {
                                pGraphLibM->forceCharacterMove(currentPlayer.instanceId, {currentPlayer.x, Config.HEIGHT_MAP_SCALE * currentPlayer.z, currentPlayer.y});
                            }
                            else
                            {
                                pGraphLibM->playAnimation(currentPlayer.instanceId, actionToAnimationConfig(currentPlayer.currentAction, {currentPlayer.x, currentPlayer.y, currentPlayer.z}));
                            }
                        }

                        if (currentPlayer.inventory != prevCurrentPlayer.inventory)
                        {
                            pGui->setInventory(currentPlayer.inventory);
                        }

                        if (currentPlayer.skills != prevCurrentPlayer.skills)
                        {
                            pGui->setSkills(currentPlayer.skills);
                        }

                        if (currentPlayer.hp != prevCurrentPlayer.hp)
                        {
                            pGui->setHp(currentPlayer.hp);
                        }
                    }

                    std::set<int> itemIdsToRemove;
                    for (const auto& itemEntry : prevGameStateMsgM.items)
                    {
                        for (const auto& item : itemEntry.second)
                        {
                            itemIdsToRemove.insert(item.instanceId);
                        }
                    }

                    for (const auto& itemEntry : gameStateMsg.items)
                    {
                        for (const auto& item : itemEntry.second)
                        {
                            // If previous message didn't have this item, draw it
                            if (!itemIdsToRemove.contains(item.instanceId))
                            {
                                const auto itemAsset = assetCacheM.getItemAssetDataById(item.id);
                                pGraphLibM->addGameObject(-item.instanceId, itemAsset.vertices, itemAsset.indices, itemAsset.texture, {itemEntry.first.x, Config.HEIGHT_MAP_SCALE * itemEntry.first.z, itemEntry.first.y}, 0);
                            }

                            // Do not remove item as it is in the current gamestate message
                            itemIdsToRemove.erase(item.instanceId);
                        }
                    }

                    // Remove all the leftover items from the graphics backend
                    for (const auto& itemId : itemIdsToRemove)
                    {
                        std::cout << "removing item object@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
                        pGraphLibM->removeGameObject(itemId);
                    }

                    std::set<int> entityIdsToRemove;
                    for (const auto& entity : prevGameStateMsgM.entities)
                    {
                        // Do not remove the current player
                        if (entity.instanceId == currentPlayer.instanceId)
                        {
                            continue;
                        }
                        entityIdsToRemove.insert(entity.instanceId);
                    }

                    for (const auto& entity : gameStateMsg.entities)
                    {
                        // Do not draw the current player twice
                        if (entity.instanceId == currentPlayer.instanceId)
                        {
                            if (entity.equipment != pGui->getEquipment())
                            {
                                pGui->setEquipment(entity.equipment);
                            }
                            continue;
                        }

                        // Do not remove entity as it is in the current gamestate message
                        entityIdsToRemove.erase(entity.instanceId);

                        // Find entity from the previous gamestate message
                        const auto& prevEntity = std::find_if(prevGameStateMsgM.entities.begin(), prevGameStateMsgM.entities.end(), [&entity](const auto& e)
                                                              { return e.instanceId == entity.instanceId; });

                        // If the entity is not found, add it to the graphics backend
                        if (prevEntity == prevGameStateMsgM.entities.end())
                        {
                            const auto& charAsset = assetCacheM.getCharacterAssetDataById(entity.id);
                            pGraphLibM->addCharacterObject(entity.instanceId, charAsset.vertices, charAsset.indices, charAsset.texture, charAsset.animations, charAsset.bones, charAsset.boneTransformations, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                        }
                        else
                        {
                            // If the entity is found, check if it has changed action
                            if (!entity.hasSameActionAs(*prevEntity))
                            {
                                pGraphLibM->playAnimation(entity.instanceId, actionToAnimationConfig(entity.currentAction, {entity.x, entity.y, entity.z}));

                                // Move action is special case as we need to animate and move the entity at the same time
                                if (entity.currentAction.id == ActionType::Move)
                                {
                                    const auto& coords = entity.currentAction.targetCoordinate;
                                    pGraphLibM->moveCharacter(entity.instanceId, {coords.x, Config.HEIGHT_MAP_SCALE * coords.z, coords.y}, entity.currentAction.durationMs / 1000.0f);
                                }
                                else if (entity.currentAction.id == ActionType::Respawn)
                                {
                                    pGraphLibM->forceCharacterMove(entity.instanceId, {entity.x, Config.HEIGHT_MAP_SCALE * entity.z, entity.y});
                                }
                            }
                        }
                    }

                    // Remove all the leftover entities from the graphics backend
                    for (const auto& entityId : entityIdsToRemove)
                    {
                        pGraphLibM->removeCharacterObject(entityId);
                    }

                    // Initialize objects only once. If the previous gamestate message is empty, we know that this is the first gamestate message. Therefore we initialize the objects. For now the server doesn't update the objects after startup.
                    if (prevGameStateMsgM.objects.empty())
                    {
                        for (const auto& object : gameStateMsg.objects)
                        {
                            const auto objAsset = assetCacheM.getObjectAssetDataById(object.id);
                            // Convert rotation 0, 1, 2, 3 to 0, pi/2, pi, 3pi/2
                            const float rotation = object.rotation * (glm::pi<float>() / 2);
                            pGraphLibM->addGameObject(std::stoi(object.instanceId), objAsset.vertices, objAsset.indices, objAsset.texture, {object.x, Config.HEIGHT_MAP_SCALE * object.z, object.y}, rotation);
                        }
                    }

                    prevGameStateMsgM = gameStateMsg;
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
            const int hitX = std::roundf(hit.hitPoint.x);
            const int hitZ = std::roundf(hit.hitPoint.z);

			std::cout << "Hit at " << hitX << " " << hitZ << std::endl;
			switch (hit.objectType)
			{
				case ObjectType::AREA:
					std::cout << "Area: " << std::endl;
                    co_await move(hitX, hitZ);
					break;
				case ObjectType::CHARACTER:
					std::cout << "Character: " << hit.id << std::endl;
					if (hit.button == GLFW_MOUSE_BUTTON_LEFT)
					{
						co_await attack(hit.id);
					}
					else if (hit.button == GLFW_MOUSE_BUTTON_RIGHT)
					{
						co_await talk(hit.id);
					}
					break;
				case ObjectType::GAMEOBJECT:
                    if (hit.id >= 0)
                    {
                        co_await interactWithObject(hit.id);
                        std::cout << "Game object: " << hit.id << std::endl;
                    }
                    else 
                    {
                        co_await pickUpItem(hit.id, hitX, hitZ);
                        std::cout << "Item: " << hit.id << std::endl;
                    }
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

    // Load assets to memory
    assetCacheM.loadAssets();
    // init graphics library
    pGraphLibM = new RehtiGraphics();

    // Create map bounding box
    std::vector<std::vector<int>> heightMatrix = fetchHeightMatrix();
    std::vector<std::vector<std::string>> areaMatrix = fetchAreaMatrix();

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
    pGraphLibM->startMainLoop();
}
