#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
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

boost::asio::awaitable<bool> Client::connect()
{
  try
  {
    std::cout << "Connecting to server..." << std::endl;
    co_await connectionM->connectToServer(endpointsM);
    boost::asio::co_spawn(ioContextM, connectionM->listenForMessages(), boost::asio::detached);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    co_return false;
  }
  co_return true;
}

boost::asio::awaitable<void> Client::randomWalk()
{
  if (connectionM->isConnected())
  {
    MoveMessage msg;
    msg.x = std::rand() % 10;
    msg.y = std::rand() % 10;
    msg.z = std::rand() % 10;
    co_await connectionM->send(MessageApi::createMove(msg));
  }
}

void Client::test()
{
  boost::asio::co_spawn(
      ioContextM, [this]() -> boost::asio::awaitable<void>
      {
    co_await connect();
    while (true)
    {
      co_await randomWalk();
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    } },
      boost::asio::detached);

  thrContextM = std::thread([this]()
                            { ioContextM.run(); });
}

void Client::processMessages()
{
  while (true)
  {
    if (!messagesM.empty())
    {
      Message msg = messagesM.pop_front();
      std::cout << msg.getBody() << std::endl;
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}