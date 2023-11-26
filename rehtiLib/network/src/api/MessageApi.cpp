#include "MessageApi.hpp"
#include "RehtiUtils.hpp"

const MessageStruct MessageApi::createLogin(const LoginMessage& login)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("username", rapidjson::StringRef(login.username.c_str()), allocator);
    document.AddMember("password", rapidjson::StringRef(login.password.c_str()), allocator);

    return MessageStruct{login.id, createString(document)};
};

LoginMessage MessageApi::parseLogin(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "username", ValueType::STRING) || !validMember(document, "password", ValueType::STRING))
    {
        throw std::runtime_error("Invalid login message");
    }

    LoginMessage login;
    login.username = document["username"].GetString();
    login.password = document["password"].GetString();

    return login;
};

const MessageStruct MessageApi::createMove(const MoveMessage& move)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("x", move.x, allocator);
    document.AddMember("y", move.y, allocator);

    return MessageStruct{move.id, createString(document)};
};

MoveMessage MessageApi::parseMove(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "y", ValueType::INT) || !validMember(document, "x", ValueType::INT))
    {
        throw std::runtime_error("Invalid move message");
    }

    MoveMessage move;
    move.x = document["x"].GetInt();
    move.y = document["y"].GetInt();

    return move;
};

MessageStruct MessageApi::createAttack(const AttackMessage& attack)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("targetId", attack.targetId, allocator);

    return MessageStruct{attack.id, createString(document)};
};

AttackMessage MessageApi::parseAttack(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "targetId", ValueType::INT))
    {
        throw std::runtime_error("Invalid attack message");
    }

    AttackMessage attack;
    attack.targetId = document["targetId"].GetInt();

    return attack;
};

MessageStruct MessageApi::createObjectInteract(const ObjectInteractMessage& objectInteract)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("objectId", rapidjson::StringRef(objectInteract.objectId.c_str()), allocator);

    return MessageStruct{objectInteract.id, createString(document)};
};

ObjectInteractMessage MessageApi::parseObjectInteract(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "objectId", ValueType::STRING))
    {
        throw std::runtime_error("Invalid object interact message");
    }

    ObjectInteractMessage objectInteract;
    objectInteract.objectId = document["objectId"].GetString();

    return objectInteract;
};

MessageStruct MessageApi::createUseItem(const UseItemMessage& useItemMsg)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("itemId", useItemMsg.itemId, allocator);

    return MessageStruct{useItemMsg.id, createString(document)};
};

UseItemMessage MessageApi::parseUseItem(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "itemId", ValueType::INT))
    {
        throw std::runtime_error("Invalid UseItem message");
    }

    UseItemMessage useItemMsg;
    useItemMsg.itemId = document["itemId"].GetInt();

    return useItemMsg;
};

MessageStruct MessageApi::createUnequip(const UnequipMessage& unequipMsg)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("itemId", unequipMsg.itemId, allocator);

    return MessageStruct{unequipMsg.id, createString(document)};
};

UnequipMessage MessageApi::parseUnequip(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "itemId", ValueType::INT))
    {
        throw std::runtime_error("Invalid Unequip message");
    }

    UnequipMessage unequipMsg;
    unequipMsg.itemId = document["itemId"].GetInt();

    return unequipMsg;
};

MessageStruct MessageApi::createGameState(const GameStateMessage& gameState)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    // Add entities
    rapidjson::Value entityArray(rapidjson::kArrayType);
    for (const auto& entity : gameState.entities)
    {
        rapidjson::Value entityObject(rapidjson::kObjectType);
        entityObject.AddMember("entityId", entity.entityId, allocator);
        entityObject.AddMember("name", rapidjson::StringRef(entity.name.c_str()), allocator);
        entityObject.AddMember("x", entity.x, allocator);
        entityObject.AddMember("y", entity.y, allocator);
        entityObject.AddMember("z", entity.z, allocator);
        entityObject.AddMember("hp", entity.hp, allocator);
        entityObject.AddMember("currentActionType", entity.currentActionType, allocator);
        rapidjson::Value equipment(rapidjson::kArrayType);
        for (const auto& item : entity.equipment)
        {
            rapidjson::Value itemObject(rapidjson::kObjectType);
            itemObject.AddMember("id", item.id, allocator);
            itemObject.AddMember("instanceId", item.instanceId, allocator);
            itemObject.AddMember("name", rapidjson::StringRef(item.name.c_str()), allocator);
            itemObject.AddMember("stackSize", item.stackSize, allocator);
            equipment.PushBack(itemObject, allocator);
        }
        entityObject.AddMember("equipment", equipment, allocator);
        entityArray.PushBack(entityObject, allocator);
    }
    document.AddMember("entities", entityArray, allocator);

    // Add objects
    rapidjson::Value objectArray(rapidjson::kArrayType);
    for (const auto& object : gameState.objects)
    {
        rapidjson::Value objectObject(rapidjson::kObjectType);
        objectObject.AddMember("id", object.id, allocator);
        objectObject.AddMember("instanceId", rapidjson::StringRef(object.instanceId.c_str()), allocator);
        objectObject.AddMember("x", object.x, allocator);
        objectObject.AddMember("y", object.y, allocator);
        objectObject.AddMember("z", object.z, allocator);
        objectObject.AddMember("rotation", object.rotation, allocator);
        objectArray.PushBack(objectObject, allocator);
    }
    document.AddMember("objects", objectArray, allocator);

    // Add current player
    rapidjson::Value currentPlayer(rapidjson::kObjectType);
    currentPlayer.AddMember("entityId", gameState.currentPlayer.entityId, allocator);
    currentPlayer.AddMember("name", rapidjson::StringRef(gameState.currentPlayer.name.c_str()), allocator);
    currentPlayer.AddMember("x", gameState.currentPlayer.x, allocator);
    currentPlayer.AddMember("y", gameState.currentPlayer.y, allocator);
    currentPlayer.AddMember("z", gameState.currentPlayer.z, allocator);
    currentPlayer.AddMember("hp", gameState.currentPlayer.hp, allocator);
    currentPlayer.AddMember("currentActionType", gameState.currentPlayer.currentActionType, allocator);
    rapidjson::Value skills(rapidjson::kArrayType);
    for (const auto& skill : gameState.currentPlayer.skills)
    {
        rapidjson::Value skillObject(rapidjson::kObjectType);
        skillObject.AddMember("id", skill.id, allocator);
        skillObject.AddMember("name", rapidjson::StringRef(skill.name.c_str()), allocator);
        skillObject.AddMember("xp", skill.xp, allocator);
        skills.PushBack(skillObject, allocator);
    }
    currentPlayer.AddMember("skills", skills, allocator);
    rapidjson::Value inventory(rapidjson::kArrayType);
    for (const auto& item : gameState.currentPlayer.inventory)
    {
        rapidjson::Value itemObject(rapidjson::kObjectType);
        itemObject.AddMember("id", item.id, allocator);
        itemObject.AddMember("instanceId", item.instanceId, allocator);
        itemObject.AddMember("name", rapidjson::StringRef(item.name.c_str()), allocator);
        itemObject.AddMember("stackSize", item.stackSize, allocator);
        inventory.PushBack(itemObject, allocator);
    }
    currentPlayer.AddMember("inventory", inventory, allocator);

    document.AddMember("currentPlayer", currentPlayer, allocator);

    return MessageStruct{gameState.id, createString(document)};
};

GameStateMessage MessageApi::parseGameState(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    // No need to validate the message body here, since the server is the only one sending game state messages.
    // The server is trusted to send valid messages.

    GameStateMessage gameState;

    gameState.entities = std::vector<GameStateEntity>();
    for (const auto& entity : document["entities"].GetArray())
    {
        GameStateEntity gameStateEntity;
        gameStateEntity.entityId = entity["entityId"].GetInt();
        gameStateEntity.name = entity["name"].GetString();
        gameStateEntity.x = entity["x"].GetInt();
        gameStateEntity.y = entity["y"].GetInt();
        gameStateEntity.z = entity["z"].GetInt();
        gameStateEntity.hp = entity["hp"].GetInt();
        gameStateEntity.currentActionType = entity["currentActionType"].GetInt();

        gameStateEntity.equipment = std::vector<GameItem>();
        for (const auto& item : entity["equipment"].GetArray())
        {
            GameItem itemObject;
            itemObject.id = item["id"].GetInt();
            itemObject.instanceId = item["instanceId"].GetInt();
            itemObject.name = item["name"].GetString();
            itemObject.stackSize = item["stackSize"].GetInt();

            gameStateEntity.equipment.push_back(itemObject);
        }

        gameState.entities.push_back(gameStateEntity);
    }

    gameState.objects = std::vector<GameStateObject>();
    for (const auto& object : document["objects"].GetArray())
    {
        GameStateObject gameStateObject;
        gameStateObject.id = object["id"].GetInt();
        gameStateObject.instanceId = object["instanceId"].GetString();
        gameStateObject.x = object["x"].GetInt();
        gameStateObject.y = object["y"].GetInt();
        gameStateObject.z = object["z"].GetInt();
        gameStateObject.rotation = object["rotation"].GetUint();

        gameState.objects.push_back(gameStateObject);
    }

    gameState.currentPlayer.entityId = document["currentPlayer"]["entityId"].GetInt();
    gameState.currentPlayer.name = document["currentPlayer"]["name"].GetString();
    gameState.currentPlayer.x = document["currentPlayer"]["x"].GetInt();
    gameState.currentPlayer.y = document["currentPlayer"]["y"].GetInt();
    gameState.currentPlayer.z = document["currentPlayer"]["z"].GetInt();
    gameState.currentPlayer.hp = document["currentPlayer"]["hp"].GetInt();
    gameState.currentPlayer.currentActionType = document["currentPlayer"]["currentActionType"].GetInt();
    gameState.currentPlayer.skills = std::vector<Skill>();

    for (const auto& skill : document["currentPlayer"]["skills"].GetArray())
    {
        Skill skillObject;
        skillObject.id = skill["id"].GetInt();
        skillObject.name = skill["name"].GetString();
        skillObject.xp = skill["xp"].GetInt();

        gameState.currentPlayer.skills.push_back(skillObject);
    }

    gameState.currentPlayer.inventory = std::vector<GameItem>();
    for (const auto& item : document["currentPlayer"]["inventory"].GetArray())
    {
        GameItem itemObject;
        itemObject.id = item["id"].GetInt();
        itemObject.instanceId = item["instanceId"].GetInt();
        itemObject.name = item["name"].GetString();
        itemObject.stackSize = item["stackSize"].GetInt();

        gameState.currentPlayer.inventory.push_back(itemObject);
    }

    return gameState;
};

MessageStruct MessageApi::createInformative(const InformativeMessage& informative)
{
    rapidjson::Document document = createDocument();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("message", rapidjson::StringRef(informative.message.c_str()), allocator);

    return MessageStruct{informative.id, createString(document)};
};

InformativeMessage MessageApi::parseInformative(std::string msgBody)
{
    rapidjson::Document document = parseDocument(msgBody);

    if (!validMember(document, "message", ValueType::STRING))
    {
        throw std::runtime_error("Invalid informative message");
    }

    InformativeMessage informative;
    informative.message = document["message"].GetString();

    return informative;
};
