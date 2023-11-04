#include <chrono>
#include <iostream>
#include <thread>

#include "Client.hpp"
#include "graphics.h"
#include "network.h"
#include "RehtiReader.hpp"

int main(int argc, char *argv[])
{
  RehtiGraphics graphLib = RehtiGraphics();
  std::vector<std::vector<int>> heightMatrix;
  std::vector<std::vector<std::string>> areaMatrix;
  loadHeightMap(heightMatrix, Config.GENERATED_HEIGHT_MAP_PATH);
  loadAreaMap(areaMatrix, Config.GENERATED_AREA_MAP_PATH);
  graphLib.addMapBoundingBox(MapAABBData{heightMatrix, areaMatrix, Config.AREA_WIDTH, Config.HEIGHT_MAP_SCALE, Config.TILE_SIDE_SCALE, Config.TILE_SIDE_UNIT});
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
    client.start();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}