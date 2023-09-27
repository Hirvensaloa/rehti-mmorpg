#pragma once

#include <boost/asio/awaitable.hpp>

#include <network.h>

class Client
{
public:
  Client(std::string ip, std::string port);

  awaitable<bool> connect();

  awaitable<void> sayHello();

  void test();

private:
  boost::asio::io_context ioContextM;
  boost::asio::ip::tcp::resolver resolverM;
  boost::asio::ip::tcp::resolver::results_type endpointsM;

  MessageQueue messagesM;
  std::unique_ptr<Connection> connectionM;

  std::thread thrContextM;
};