#pragma once

#include <network.h>

class Client {
public:
  Client(std::string ipP, std::string portP);

  bool connect();

  void sayHello();

private:
  boost::asio::io_context ioContextM;
  boost::asio::ip::tcp::resolver resolverM;
  boost::asio::ip::tcp::resolver::results_type endpointsM;
  boost::asio::ip::tcp::socket socketM;

  MessageQueue<Message> messagesM;
  std::unique_ptr<Connection> connectionM;

  std::thread thrContextM;
};