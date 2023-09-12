#pragma once

#include "Client.hpp"

#include <iostream>
#include <memory>

Client::Client(std::string ipP, std::string portP)
    : ioContextM(boost::asio::io_context()),
      resolverM(boost::asio::ip::tcp::resolver(ioContextM)),
      endpointsM(resolverM.resolve(ipP, portP)),
      socketM(boost::asio::ip::tcp::socket(ioContextM)),
      messagesM(MessageQueue<Message>()),
      connectionM(std::make_unique<Connection>(
          Connection::owner::client, ioContextM, socketM, messagesM)){};

bool Client::connect()
{
  try
  {
    connectionM->connectToServer(endpointsM);
    thrContextM = std::thread([this]()
                              { ioContextM.run(); });
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return false;
  }
  return true;
}

void Client::sayHello()
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
    connectionM->send(msg);
  }
}