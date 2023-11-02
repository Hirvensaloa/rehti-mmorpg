#pragma once

#include <boost/asio/awaitable.hpp>

#include <network.h>

class Client
{
public:
  Client(std::string ip, std::string port);

  boost::asio::awaitable<bool> connect();

  void start();

  /**
   * @brief Prompts user for login information and sends a LoginMessage with that information. Currently only username used for identification
   * @return True if user inputs a valid username, false otherwise.
   */
  boost::asio::awaitable<bool> login();

  void processMessages();

private:
  boost::asio::io_context ioContextM;
  boost::asio::ip::tcp::resolver resolverM;
  boost::asio::ip::tcp::resolver::results_type endpointsM;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;

  MessageQueue messagesM;
  std::unique_ptr<Connection> connectionM;

  std::thread connectionThreadM;
  std::thread ioThreadM;
};