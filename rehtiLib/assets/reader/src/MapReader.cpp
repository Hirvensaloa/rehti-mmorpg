#include <string>
#include <vector>
#include <cstdint>

#include "Utils.hpp"
#include "MapReader.hpp"

void loadAccessMap(std::vector<std::vector<uint8_t>> &accessMap, std::string path)
{
  std::vector<std::vector<std::string>> accessMapString;
  readMatrixFromFile(accessMapString, path);

  for (auto &row : accessMapString)
  {
    std::vector<uint8_t> rowUint8;
    for (auto &col : row)
    {
      rowUint8.push_back(std::stoi(col));
    }
    accessMap.push_back(rowUint8);
  }
}