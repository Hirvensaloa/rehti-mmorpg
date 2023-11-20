#include <chrono>
#include <iostream>
#include <thread>

#include "Client.hpp"
#include "network.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        Client client = Client(argv[1], "9999");
        client.start();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}