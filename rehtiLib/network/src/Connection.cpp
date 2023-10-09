#include <condition_variable>
#include <iostream>
#include <memory>

#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/system.hpp>

#include "Connection.hpp"

Connection::Connection(owner parent, boost::asio::io_context &context,
                       boost::asio::ip::tcp::socket socket, MessageQueue &inc)
    : rAsioContextM(context), socketM(std::move(socket)), rIncomingMessagesM(inc)
{
  ownertypeM = parent;
}

Connection::~Connection() {}

uint32_t Connection::getID() const { return idM; }

boost::asio::awaitable<void> Connection::listenForMessages()
{
  while (isConnected())
  {
    co_await readMessage();
  }
  std::cout << "Client disconnected" << std::endl;
}

boost::asio::awaitable<bool> Connection::connectToServer(const boost::asio::ip::tcp::resolver::results_type &endpoints)
{
  if (ownertypeM != owner::client)
  {
    co_return false;
  }

  try
  {
    boost::system::error_code ec;
    co_await boost::asio::async_connect(socketM, endpoints, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

    if (ec)
    {
      std::cout << "Failed to connect to server: " << ec.message() << std::endl;
      co_return false;
    }

    std::cout << "Connected to server: " << socketM.remote_endpoint().address().to_string() << std::endl;

    co_return true;
  }
  catch (std::exception &e)
  {
    std::cerr << "Client Connection Error: " << e.what() << "\n";
    co_return false;
  }
}

bool Connection::connectToClient(uint32_t userid)
{
  if (socketM.is_open())
  {
    idM = userid;
    return true;
  }

  return false;
}

bool Connection::isConnected() const { return socketM.is_open(); }

void Connection::disconnect()
{
  socketM.close();
}

boost::asio::awaitable<void> Connection::send(const MessageStruct msg)
{
  co_await writeMessage(Message(msg));
}

boost::asio::awaitable<void> Connection::writeMessage(const Message msg)
{
  std::cout << idM << ": Writing message..." << msg.getBody() << " size: " << msg.getSize() << std::endl;
  boost::system::error_code ec;
  // 1. Write header
  co_await boost::asio::async_write(
      socketM,
      boost::asio::buffer(&msg.getHeader(), sizeof(msg_header)),
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));

  // 2. Write body, if there is one
  if (msg.getSize() > 0 && !ec)
  {
    co_await boost::asio::async_write(
        socketM,
        boost::asio::buffer(msg.getBody().data(), msg.getSize()),
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  }

  if (ec)
  {
    std::cout << idM << ": Write failed" << std::endl;
    disconnect();
  }
  else
  {
    std::cout << idM << ": Write successful" << std::endl;
  }
}

boost::asio::awaitable<void> Connection::readMessage()
{
  std::cout << idM << ": Reading message..." << std::endl;
  // 1. Wait for header to arrive and then read it
  msg_header tempHeaderM;
  co_await boost::asio::async_read(socketM, boost::asio::buffer(&tempHeaderM, sizeof(msg_header)), boost::asio::use_awaitable);

  // 2. If there is a body, read the body
  char tempBodyM[128] = {0};
  if (tempHeaderM.size > 0)
  {
    co_await boost::asio::async_read(socketM, boost::asio::buffer(&tempBodyM[0], tempHeaderM.size), boost::asio::use_awaitable);
  }

  // 3. Add message to incoming queue
  if (ownertypeM == owner::server)
  {
    rIncomingMessagesM.push_back(Message(this->shared_from_this(), tempHeaderM, std::move(std::string(tempBodyM))));
  }
  else
  {
    rIncomingMessagesM.push_back(Message(nullptr, tempHeaderM, std::move(std::string(tempBodyM))));
  }
}
