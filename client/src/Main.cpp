#include <chrono>
#include <iostream>
#include <thread>

#include "Client.hpp"
#include "graphics.h"
#include "network.h"

int main(int argc, char *argv[])
{
    RehtiGraphics graphLib = RehtiGraphics();
    graphLib.demo();
  std::cout << "Winner winner chicken dinner" << std::endl;
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    Client client = Client(argv[1], "9999");
    client.test();
    client.processMessages();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}