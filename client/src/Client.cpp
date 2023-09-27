#pragma once

#include <iostream>
#include <memory>

#include "Client.hpp"

Client::Client(std::string ip, std::string port)
    : ioContextM(boost::asio::io_context()),
      resolverM(boost::asio::ip::tcp::resolver(ioContextM)),
      endpointsM(resolverM.resolve(ip, port)),
      messagesM(MessageQueue()),
      connectionM(std::make_unique<Connection>(
          Connection::owner::client, ioContextM, std::move(boost::asio::ip::tcp::socket(ioContextM)), messagesM)){};

awaitable<bool> Client::connect()
{
  try
  {
    std::cout << "Connecting to server..." << std::endl;
    co_await connectionM->connectToServer(endpointsM);
    asio::co_spawn(ioContextM, connectionM->listenForMessages(), asio::detached);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    co_return false;
  }
  co_return true;
}

awaitable<void> Client::sayHello()
{
  if (connectionM->isConnected())
  {
    msg_header header;
    std::stringstream ss;

    ss << "Hello from big C!\0";
    std::string s = ss.str();
    header.size = sizeof(s) + s.size();
    header.id = 111;
    Message msg = Message(nullptr, header, s);
    std::cout << "sending: " << msg.getBody() << ", size " << header.size << std::endl;
    co_await connectionM->send(msg);
  }
}

void Client::test()
{
  boost::asio::co_spawn(ioContextM, [this]() -> awaitable<void> {
    co_await connect();
    while (true)
    {
      co_await sayHello();
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
  }, boost::asio::detached);

  ioContextM.run();
}