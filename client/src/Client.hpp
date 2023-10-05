#pragma once

#include <boost/asio/awaitable.hpp>

#include <network.h>

class Client
{
public:
  Client(std::string ip, std::string port);

  boost::asio::awaitable<bool> connect();

  boost::asio::awaitable<void> randomWalk();

  void test();

  void processMessages();

private:
  boost::asio::io_context ioContextM;
  boost::asio::ip::tcp::resolver resolverM;
  boost::asio::ip::tcp::resolver::results_type endpointsM;

  MessageQueue messagesM;
  std::unique_ptr<Connection> connectionM;

  std::thread thrContextM;
};