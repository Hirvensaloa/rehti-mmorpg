#pragma once

#include "Message.hpp"
#include "MessageQueue.hpp"
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection>
{

public:
  enum owner
  {
    server,
    client
  };

  Connection(owner parent, boost::asio::io_context &context,
             boost::asio::ip::tcp::socket socket, MessageQueue<Message> &inc)
      : rAsioContextM(context), socketM(std::move(socket)), rIncomingMessagesM(inc)
  {
    ownertypeM = parent;
  }

  ~Connection() {}

  uint32_t getID() const { return idM; }

  void connectToServer(const boost::asio::ip::tcp::resolver::results_type
                           &endpoints)
  { // Connects to server
    if (ownertypeM == owner::client)
    {
      boost::asio::async_connect(
          socketM, endpoints,
          [this](std::error_code errorcode,
                 boost::asio::ip::tcp::endpoint endpoint)
          {
            if (!errorcode)
              readHeader();
          });
    }
  }

  void connectToClient(uint32_t userid = 0)
  { // Connects to client

    if (ownertypeM == owner::server)
    {
      if (socketM.is_open())
      {
        idM = userid;
        readHeader();
      }
    }
  }

  bool isConnected() const { return socketM.is_open(); }

  void disconnect()
  {
    if (isConnected())
      boost::asio::post(rAsioContextM, [this]()
                        { socketM.close(); });
  }

  void send(const Message &msg)
  {
    boost::asio::post(rAsioContextM, [this, msg]()
                      {
      bool writingMessage = !outgoingMessagesM.empty();
      outgoingMessagesM.push_back(msg);
      if (!writingMessage)
        writeHeader(); });
  }

private:
  void writeHeader()
  {
    boost::asio::async_write(
        socketM,
        boost::asio::buffer(&outgoingMessagesM.front().getHeader(),
                            sizeof(msg_header)),
        [this](std::error_code errorcode, std::size_t length)
        {
          if (!errorcode)
          {
            if (outgoingMessagesM.front().getSize() > 0)
            {
              std::cout << outgoingMessagesM.front().getBody() << std::endl;
              writeBody();
            }
            else
            {
              outgoingMessagesM.pop_front();
              if (!outgoingMessagesM.empty())
              {
                writeHeader();
              }
            }
          }
          else
          {
            std::cout << idM << ": Write Header failed." << std::endl;
            socketM.close();
          }
        });
  }

  void writeBody()
  {
    boost::asio::async_write(
        socketM,
        boost::asio::buffer(outgoingMessagesM.front().getBody().data(),
                            outgoingMessagesM.front().getSize()),
        [this](std::error_code errorcode, std::size_t length)
        {
          if (!errorcode)
          {
            outgoingMessagesM.pop_front();
            if (!outgoingMessagesM.empty())
            {
              writeHeader();
            }
          }
          else
          {
            std::cout << idM << ": Write Body failed." << std::endl;
          }
        });
  }

  void readHeader()
  {
    using namespace boost::placeholders;
    boost::asio::async_read(
        socketM, boost::asio::buffer(&tempHeaderM, sizeof(msg_header)),
        boost::bind(&Connection::handleHeader,
                    this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }

  void handleHeader(const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    if (!error)
    {
      if (tempHeaderM.size > 0)
      {
        readBody();
      }
      else
      {
        addToIncomingMessages();
      }
    }
    else
    {
      std::cout << idM << "Reading header failed." << std::endl;
      socketM.close();
    }
  }

  void readBody()
  {
    using namespace boost::placeholders;
    boost::asio::async_read(
        socketM, boost::asio::buffer(&tempBodyM[0], tempHeaderM.size), boost::bind(&Connection::handleBody, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }

  void handleBody(const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    if (!error)
    {
      // std::cout << "transferred: " << bytes_transferred << std::endl;
      // std::cout.write(tempBodyM, strlen(tempBodyM));
      addToIncomingMessages();
    }
    else
    {
      std::cout << idM << " Reading body failed." << std::endl;
      socketM.close();
    }
  }

  void addToIncomingMessages()
  {
    if (ownertypeM == owner::server)
    {
      rIncomingMessagesM.push_back(
          Message(nullptr, tempHeaderM, std::move(std::string(tempBodyM))));
    }
    else
    {
      rIncomingMessagesM.push_back(Message(nullptr, tempHeaderM, std::move(std::string(tempBodyM))));
    }
    readHeader();
  }

protected:
  boost::asio::io_context &rAsioContextM;
  boost::asio::ip::tcp::socket socketM;

  MessageQueue<Message> outgoingMessagesM;
  MessageQueue<Message> &rIncomingMessagesM;

  msg_header tempHeaderM;
  char tempBodyM[128] = {0};
  owner ownertypeM = owner::server;

  uint32_t idM = 0;
};