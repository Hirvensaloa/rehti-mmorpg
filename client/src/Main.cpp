#include <chrono>
#include <iostream>
#include <thread>

#include "Client.hpp"
#include "graphics.h"
#include "network.h"

int main(int argc, char *argv[])
{
  testFunc(); // should be available in test.hpp. That in turn includes glfw
              // library
  std::cout << "Winner winner chicken dinner" << std::endl;
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    Client testiClient = Client(argv[1], "9999");
    testiClient.test();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}