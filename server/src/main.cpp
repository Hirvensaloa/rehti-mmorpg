#include "database/connect.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <network.h>
#include <thread>

using namespace boost::asio;

uint16_t PORT = 9999;

int main()
{
  io_context ioContext;
  ip::tcp::acceptor acceptor(ioContext, ip::tcp::endpoint(ip::tcp::v4(), PORT));

  std::cout << "Server started on port " << PORT << std::endl;

  while (true)
  {
    try
    {
      createConnection();

      ip::tcp::socket socket(ioContext);
      acceptor.accept(socket);
      std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
                << ":" << socket.remote_endpoint().port() << std::endl;
      MessageQueue<Message> messages = MessageQueue<Message>();
      std::unique_ptr<Connection> connection = std::make_unique<Connection>(
          Connection::owner::server, ioContext, socket, messages);
      connection->connectToClient();
      std::thread thrContext = std::thread([&]()
                                           { ioContext.run(); });
      while (connection->isConnected())
      {
        if (!messages.empty())
        {
          Message msg = messages.pop_front();
          std::cout << msg.getBody() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      std::cout << "conn gone" << std::endl;
      return 0;
    }
    catch (std::exception &e)
    {
      std::cout << "test" << std::endl;
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  return 0;
}
