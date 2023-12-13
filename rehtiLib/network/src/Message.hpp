#pragma once

#include <memory>

#include "Connection.hpp"
#include "api/MessageApi.hpp"

class Connection;

// Represents a message sent between client and server.

struct msg_header
{
    unsigned int id{};
    uint32_t size = 0; // Size of the message body
};

class Message
{
public:
    Message(std::shared_ptr<Connection> connection, msg_header header,
            const std::string& body)
        : connectionM(connection), headerM(header), bodyM(body) {}

    Message(const MessageStruct& msg)
        : connectionM(nullptr), bodyM(msg.body)
    {
        headerM.id = static_cast<int>(msg.id);
        headerM.size = msg.body.size();
    }

    const std::string& getBody() const { return bodyM; }

    const msg_header& getHeader() const { return headerM; }

    uint32_t getSize() const { return headerM.size; }

    const std::shared_ptr<Connection>& getConn() const { return connectionM; }

private:
    std::shared_ptr<Connection> connectionM;
    msg_header headerM;
    std::string bodyM;
};