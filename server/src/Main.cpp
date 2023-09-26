#include "database/Connect.hpp"
#include "Server.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <network.h>
#include <thread>

using namespace boost::asio;

int main()
{
  Server server = Server();
  server.processMessages();
}
