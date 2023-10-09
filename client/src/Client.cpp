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
      workGuardM(boost::asio::make_work_guard(ioContextM)),
      messagesM(MessageQueue()),
      connectionM(std::make_unique<Connection>(
          Connection::owner::client, ioContextM, std::move(boost::asio::ip::tcp::socket(ioContextM)), messagesM))
{
  connectionThreadM = std::thread([this]()
                                  { boost::asio::co_spawn(ioContextM, connect(), boost::asio::detached); });

  ioThreadM = std::thread([this]()
                          { ioContextM.run(); });
};

void Client::start()
{
  std::cout << "Client started" << std::endl;
  std::thread([this]()
              { test(); })
      .detach();
  processMessages();
}

boost::asio::awaitable<bool> Client::connect()
{
  try
  {
    std::cout << "Connecting to server..." << std::endl;
    const bool ret = co_await connectionM->connectToServer(endpointsM);
    std::cout << ret << std::endl;

    if (ret)
    {
      boost::asio::co_spawn(ioContextM, connectionM->listenForMessages(), boost::asio::detached);
    }

    co_return ret;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    co_return false;
  }
}

boost::asio::awaitable<void> Client::randomWalk()
{
  if (connectionM->isConnected())
  {
    MoveMessage msg;
    msg.x = std::rand() % 10;
    msg.y = std::rand() % 10;
    co_await connectionM->send(MessageApi::createMove(msg));
  }
}

void Client::test()
{
  while (true)
  {
    boost::asio::co_spawn(
        ioContextM, [this]() -> boost::asio::awaitable<void>
        { co_await randomWalk(); },
        boost::asio::detached);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
}

void Client::processMessages()
{
  while (true)
  {
    // Wait for message to arrive
    messagesM.wait();

    while (!messagesM.empty())
    {
      Message msg = messagesM.pop_front();
      std::cout << msg.getBody() << std::endl;
    }
  }
}