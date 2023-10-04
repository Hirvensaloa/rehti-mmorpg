#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Message.hpp"
#include "MessageQueue.hpp"

class Connection : public std::enable_shared_from_this<Connection>
{

public:
  enum owner
  {
    server,
    client
  };

  Connection(owner parent, boost::asio::io_context &context,
             boost::asio::ip::tcp::socket socket, MessageQueue &inc);

  ~Connection();

  uint32_t getID() const;

  boost::asio::awaitable<void> listenForMessages();
  boost::asio::awaitable<bool> connectToServer(const boost::asio::ip::tcp::resolver::results_type &endpoints);

  bool connectToClient(uint32_t userid = 0);

  bool isConnected() const;

  void disconnect();
  boost::asio::awaitable<void> send(const unsigned int headerId, const std::string msgBody);

private:
  boost::asio::awaitable<void> writeMessage(const Message msg);

  boost::asio::awaitable<void> readMessage();

protected:
  boost::asio::io_context &rAsioContextM;
  boost::asio::ip::tcp::socket socketM;

  MessageQueue &rIncomingMessagesM;

  owner ownertypeM = owner::server;

  uint32_t idM = 0;
};