#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <iostream>
#include <optional>
#include <vector>

#include "Types.hpp"

/**
 * @file CREATING NEW MESSAGES:
 *
 * 1. Add a new enum value to MessageId enum.
 * 2. Create a new struct for the message type.
 * 3. Add a new create and parse method to MessageApi class.
 * 4. Handle the message instance on the server
 */

enum MessageId
{
    GameState,
    Login,
    Move,
    Attack,
    ObjectInteract,
    UseItem,
    Unequip,
    Informative,
};

struct MessageStruct
{
    const MessageId id;
    const std::string body;
};

/**
 * @file All the possible message types between client and server are defined below as structs.
 * Each struct has an id field that is used to identify the message type.
 * The id field corresponds to the messages header id.
 */

struct GameStateMessage
{
    const MessageId id = MessageId::GameState;
    std::vector<GameStateEntity> entities;
    std::vector<GameStateObject> objects;
    CurrentPlayer currentPlayer;
};

struct LoginMessage
{
    const MessageId id = MessageId::Login;
    std::string username;
    std::string password;
};

struct MoveMessage
{
    const MessageId id = MessageId::Move;
    int x;
    int y;
};

struct AttackMessage
{
    const MessageId id = MessageId::Attack;
    int targetId;
};

struct ObjectInteractMessage
{
    const MessageId id = MessageId::ObjectInteract;
    std::string objectId;
};

struct UseItemMessage
{
    const MessageId id = MessageId::UseItem;
    int itemId;
};

struct UnequipMessage
{
    const MessageId id = MessageId::Unequip;
    int itemId;
};

/**
 * @brief Special type of message that is used by the server to inform the client about something happening outside the gamestate. For example, login failed.
 */
struct InformativeMessage
{
    const MessageId id = MessageId::Informative;
    std::string message;
};

/**
 * @note Each message type above needs to have create and parse methods.
 * @brief Helper class for constructing and parsing messages.
 *
 * The create methods take the message parameters as arguments and return a Message header id and Message body.
 *
 * The parse methods take a Message body string as an argument and return the message parameters.
 * Parse function might fail if the message body is not a valid json string. In that case an exception is thrown.
 *
 */
class MessageApi
{
public:
    /**
     * @brief Create a MessageStruct for login
     *
     * @param login LoginMessage
     * @return const MessageStruct
     */
    static const MessageStruct createLogin(const LoginMessage& login);
    /**
     * @brief Parse received login message body
     *
     * @param msgBody
     * @return LoginMessage
     */
    static LoginMessage parseLogin(std::string msgBody);

    /**
     * @brief Create a MessageStruct for move action
     *
     * @param move MoveMessage
     * @return const MessageStruct
     */
    static const MessageStruct createMove(const MoveMessage& move);
    /**
     * @brief Parse received move message body
     *
     * @param msgBody
     * @return MoveMessage
     */
    static MoveMessage parseMove(std::string msgBody);

    /**
     * @brief Create a MessageStruct for attack action
     *
     * @param attack AttackMessage
     * @return const MessageStruct
     */
    static MessageStruct createAttack(const AttackMessage& attack);
    /**
     * @brief Parse received attack message body
     *
     * @param msgBody
     * @return AttackMessage
     */
    static AttackMessage parseAttack(std::string msgBody);

    /**
     * @brief Create a MessageStruct for object interaction
     *
     * @param objectInteract ObjectInteractMessage
     * @return MessageStruct
     */
    static MessageStruct createObjectInteract(const ObjectInteractMessage& objectInteract);
    /**
     * @brief Parse received object interaction message body
     *
     * @param msgBody
     * @return ObjectInteractMessage
     */
    static ObjectInteractMessage parseObjectInteract(std::string msgBody);

    static MessageStruct createUseItem(const UseItemMessage& useItemMsg);
    static UseItemMessage parseUseItem(std::string msgBody);

    static MessageStruct createUnequip(const UnequipMessage& unequipMsg);
    static UnequipMessage parseUnequip(std::string msgBody);

    /**
     * @brief Create a MessageStruct for game state
     *
     * @param gameState GameStateMessage
     * @return MessageStruct
     */
    static MessageStruct createGameState(const GameStateMessage& gameState);

    /**
     * @brief Parse received game state message body
     *
     * @param msgBody
     * @return GameStateMessage
     */
    static GameStateMessage parseGameState(std::string msgBody);

    /**
     * @brief Create a MessageStruct for informative message
     *
     * @param informative InformativeMessage
     * @return MessageStruct
     */
    static MessageStruct createInformative(const InformativeMessage& informative);
    /**
     * @brief Parse received informative message body
     *
     * @param msgBody
     * @return InformativeMessage
     */
    static InformativeMessage parseInformative(std::string msgBody);
};