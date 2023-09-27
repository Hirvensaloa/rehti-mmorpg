#pragma once

#include <iostream>

#include "Message.hpp"
#include "MessageQueue.hpp"

namespace asio = boost::asio;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

class Connection : public std::enable_shared_from_this<Connection>
{

public:
  enum owner
  {
    server,
    client
  };

  Connection(owner parent, boost::asio::io_context &context,
             boost::asio::ip::tcp::socket socket, MessageQueue &inc)
      : rAsioContextM(context), socketM(std::move(socket)), rIncomingMessagesM(inc)
  {
    ownertypeM = parent;
  }

  ~Connection() {}

  uint32_t getID() const { return idM; }

  awaitable<void> listenForMessages() 
  {
    std::cout << "Listening for messages..." << std::endl;
    while (isConnected()) {
      std::cout << "Waiting for message..." << std::endl;
      co_await readMessage();
    }
  }

  awaitable<bool> connectToServer(const asio::ip::tcp::resolver::results_type &endpoints)
  {
    if (ownertypeM != owner::client) {
      co_return false;
    }

    try {
      boost::system::error_code ec;
      co_await asio::async_connect(socketM, endpoints, asio::redirect_error(use_awaitable, ec));

      if (ec) {
        std::cout << "Failed to connect to server: " << ec.message() << std::endl;
        co_return false;
      }

      std::cout << "Connected to server: " << socketM.remote_endpoint().address().to_string() << std::endl;

      co_return true;
    } catch (std::exception &e) {
      std::cerr << "Client Connection Error: " << e.what() << "\n";
      co_return false;
    }
  }

  bool connectToClient(uint32_t userid = 0)
  {
    if (socketM.is_open())
    {
      idM = userid;
      return true;
    }

    return false;
  }

  bool isConnected() const { return socketM.is_open(); }

  void disconnect()
  {
    socketM.close();
  }

  awaitable<void> send(const Message &msg)
  {
    co_await writeMessage(msg);
  }

private:
  awaitable<void> writeMessage(const Message &msg)
  {
    std::cout << idM << ": Writing message..." << msg.getBody() << msg.getSize() << std::endl;
    boost::system::error_code ec;
    // 1. Write header
    co_await asio::async_write(
        socketM,
        asio::buffer(&msg.getHeader(), sizeof(msg_header)),
        asio::redirect_error(use_awaitable, ec));

    // 2. Write body, if there is one
    if (msg.getSize() > 0 && !ec)
    {
      co_await asio::async_write(
          socketM,
          asio::buffer(msg.getBody().data(), msg.getSize()),
          asio::redirect_error(use_awaitable, ec));
    }

    if (ec)
    {
      std::cout << idM << ": Write failed" << std::endl;
      disconnect();
    } else {
      std::cout << idM << ": Write successful" << std::endl;  
    }

  }

  awaitable<void> readMessage()
  {
    // 1. Wait for header to arrive and then read it
    msg_header tempHeaderM;
    co_await asio::async_read(socketM, asio::buffer(&tempHeaderM, sizeof(msg_header)), use_awaitable);

    // 2. If there is a body, read the body
    char tempBodyM[128] = {0};
    if (tempHeaderM.size > 0) {
      co_await asio::async_read(socketM, asio::buffer(&tempBodyM[0], tempHeaderM.size), use_awaitable);
    }

    std::cout << idM << ": Read message: " << tempBodyM << std::endl;

    // 3. Add message to incoming queue
    rIncomingMessagesM.push_back(Message(nullptr, tempHeaderM, std::move(std::string(tempBodyM))));
  }

protected:
  asio::io_context &rAsioContextM;
  boost::asio::ip::tcp::socket socketM;

  MessageQueue &rIncomingMessagesM;

  owner ownertypeM = owner::server;

  uint32_t idM = 0;
};