#include <iostream>
#include <memory>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "Server.hpp"
#include "action/ObjectInteractAction.hpp"
#include "utils/AssetManager.hpp"

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
      gameWorldM(GameWorld()),
      dbManagerM(DatabaseManager())
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

        if (connection->isConnected())
        {
            connectionsM.push_back(std::move(connection));

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

void Server::handleMessage(const Message& msg)
{
    try
    {
        const auto body = msg.getBody();
        const auto msgId = msg.getHeader().id;
        const auto connId = msg.getConn()->getID();

        if (!msg.getConn()->isLoggedIn())
        {
            if (msgId == MessageId::Login)
            {

                std::cout << connId << " | Received login message | " << body << std::endl;
                const LoginMessage loginMsg = MessageApi::parseLogin(body);
                std::string username = loginMsg.username;

                loadPlayer(username, msg.getConn());
            }
            else
            {
                std::cout << "Non-login message received from non-logged in connection" << std::endl;
            }
        }

        else
        {
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

                std::cout << connId << "Attack message received." << std::endl;
                const AttackMessage attackMsg = MessageApi::parseAttack(body);
                std::shared_ptr<PlayerCharacter> gamer = gameWorldM.getPlayer(connId);
                std::shared_ptr<Entity> target = gameWorldM.getEntity(attackMsg.targetId);
                gamer->setAction(std::make_shared<AttackAction>(std::chrono::system_clock::now(), target, gamer));
                break;
            }
            case MessageId::ObjectInteract:
            {
                std::cout << connId << " | Object interact message received" << std::endl;
                const ObjectInteractMessage objectInteractMsg = MessageApi::parseObjectInteract(body);
                std::shared_ptr<PlayerCharacter> gamer = gameWorldM.getPlayer(connId);
                std::shared_ptr<Object> object = gameWorldM.getObjects().at(objectInteractMsg.objectId);
                gamer->setAction(std::make_shared<ObjectInteractAction>(std::chrono::system_clock::now(), object, gamer));
                break;
            }
            case MessageId::UseItem:
            {
                std::cout << connId << "UseItem message received." << std::endl;
                const UseItemMessage useItemMsg = MessageApi::parseUseItem(body);
                std::shared_ptr<PlayerCharacter> gamer = gameWorldM.getPlayer(connId);
                gamer->getInventory().useItem(useItemMsg.itemId);
                break;
            }
            case MessageId::Unequip:
            {
                std::cout << connId << "Unequip message received." << std::endl;
                const UnequipMessage unequipMsg = MessageApi::parseUnequip(body);
                std::shared_ptr<PlayerCharacter> gamer = gameWorldM.getPlayer(connId);
                for (auto entry : gamer->getEquipment().getSlotMap())
                {
                    if (entry.second != nullptr && entry.second->getInstanceId() == unequipMsg.itemId)
                    {
                        gamer->getEquipment().unequip(entry.first);
                    }
                }
                break;
            }
            default:
                // Unknown header id, ignore
                std::cout << "Unknown header id: " << msgId << std::endl;
                break;
            }
        }
    }
    catch (const std::exception& e)
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
    for (auto it = connectionsM.begin(); it != connectionsM.end();)
    {
        if (!((*it)->isConnected()) && (*it)->isLoggedIn())
        {
            int disconnectedId = (*it)->getID();
            savePlayer(disconnectedId);
            auto gamer = gameWorldM.getPlayer(disconnectedId);
            gamer->setDisconnected();
            gameWorldM.removePlayer(disconnectedId);
            it = connectionsM.erase(it);
        }
        else
        {
            it++;
        }
    }
    gameWorldM.updateGameWorld();
    std::thread(&Server::sendGameState, this).detach();
}

void Server::sendGameState()
{
    GameStateMessage msg;
    std::vector<GameStateEntity> entityVector;
    for (auto& npc : gameWorldM.getNpcs())
    {
        GameStateEntity entity;
        const Coordinates location = npc->getLocation();
        entity.entityId = npc->getId();
        entity.name = npc->getName();
        entity.x = location.x;
        entity.y = location.y;
        entity.z = location.z;
        entity.hp = npc->getHp();
        std::vector<GameItem> equipmentVector;
        for (auto entry : npc->getEquipment().getSlotMap())
        {
            if (entry.second != nullptr)
            {
                GameItem gameItem = {entry.second->getId(), entry.second->getInstanceId(), entry.second->getName(), entry.second->getStackSize()};
                equipmentVector.push_back(gameItem);
            }
            else
            {
                GameItem gameItem = {-1, -1, "NoItem", 0};
                equipmentVector.push_back(gameItem);
            }
        }
        entity.equipment = equipmentVector;
        entityVector.push_back(entity);
    }

    for (auto& player : gameWorldM.getPlayers())
    {
        GameStateEntity entity;
        const Coordinates location = player->getLocation();
        entity.entityId = player->getId();
        entity.name = player->getName();
        entity.x = location.x;
        entity.y = location.y;
        entity.z = location.z;
        entity.hp = player->getHp();

        std::vector<GameItem> equipmentVector;
        for (auto entry : player->getEquipment().getSlotMap())
        {
            if (entry.second != nullptr)
            {
                GameItem gameItem = {entry.second->getId(), entry.second->getInstanceId(), entry.second->getName(), entry.second->getStackSize()};
                equipmentVector.push_back(gameItem);
            }
            else
            {
                GameItem gameItem = {-1, -1, "NoItem", 0};
                equipmentVector.push_back(gameItem);
            }
        }
        entity.equipment = equipmentVector;
        entityVector.push_back(entity);
    }
    msg.entities = entityVector;

    for (auto& object : gameWorldM.getObjects())
    {
        GameStateObject gameStateObject;
        gameStateObject.id = object.second->getId();
        gameStateObject.instanceId = object.second->getInstanceId();
        const Coordinates location = object.second->getLocation();
        gameStateObject.x = location.x;
        gameStateObject.y = location.y;
        gameStateObject.z = location.z;
        gameStateObject.rotation = object.second->getRotation();
        msg.objects.push_back(gameStateObject);
    }

    for (auto& conn : connectionsM)
    {
        if (conn->isLoggedIn())
        {
            // Add the current player to the message e.g. the player that is connected to this connection
            std::shared_ptr<PlayerCharacter> player = gameWorldM.getPlayer(conn->getID());
            msg.currentPlayer.entityId = player->getId();
            msg.currentPlayer.name = player->getName();
            const Coordinates location = player->getLocation();
            msg.currentPlayer.x = location.x;
            msg.currentPlayer.y = location.y;
            msg.currentPlayer.z = location.z;
            msg.currentPlayer.hp = player->getHp();
            if (player->getCurrentAction() != nullptr)
            {
                msg.currentPlayer.currentActionType = player->getCurrentAction()->getActionType();
            }
            else
            {
                msg.currentPlayer.currentActionType = Action::ActionType::None;
            }

            const auto skills = player->getSkillSet().getSkills();

            std::vector<Skill> skillVector;
            for (auto& skill : skills)
            {
                Skill s = {skill.first, skill.second.name, skill.second.xp};
                skillVector.push_back(s);
            }
            msg.currentPlayer.skills = skillVector;

            std::vector<GameItem> inventory;
            for (auto& item : player->getInventory().getItems())
            {
                GameItem gameItem = {item->getId(), item->getInstanceId(), item->getName(), item->getStackSize()};
                inventory.push_back(gameItem);
            }
            msg.currentPlayer.inventory = inventory;

            if (conn->isConnected())
            {
                boost::asio::co_spawn(ioContextM, conn->send(MessageApi::createGameState(msg)), boost::asio::detached);
            }
        }
    }
}

void Server::initGameState()
{
    gameWorldM.initWorld();
}

void Server::loadPlayer(std::string username, const std::shared_ptr<Connection>& connection)
{
    auto data = dbManagerM.loadPlayerDataFromDb(username);
    connection->connectToClient(data.id);
    gameWorldM.addPlayer(data.username, data.id, Coordinates(data.position_x, data.position_y));

    Inventory& inventory = gameWorldM.getPlayer(data.id)->getInventory();
    std::vector<int> itemIds = dbManagerM.loadInventoryDataFromDb(data.id);
    for (auto id : itemIds)
    {
        inventory.addItem(AssetManager::createItemInstance(id));
    }

    Equipment& equipment = gameWorldM.getPlayer(data.id)->getEquipment();
    std::vector<int> equipmentIds = dbManagerM.loadEquipmentDataFromDb(data.id);
    for (auto id : equipmentIds)
    {
        equipment.equip(static_pointer_cast<EquippableItem>(AssetManager::createItemInstance(id)));
    }

    SkillSet& skills = gameWorldM.getPlayer(data.id)->getSkillSet();
    auto skillData = dbManagerM.loadSkillDataFromDb(data.id);
    for (auto skill : skillData)
    {
        skills.addSkillXp(skill.first, skill.second);
    }
}

void Server::savePlayer(int playerId)
{
    dbManagerM.savePlayerToDb(gameWorldM.getPlayer(playerId));
}