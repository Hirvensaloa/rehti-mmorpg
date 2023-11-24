#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Utils.hpp"
#include "../../Config.hpp"

void readMatrixFromFile(std::vector<std::vector<std::string>> &matrix, std::string filepath)
{
  std::ifstream file(filepath);
  std::string line;

  if (!file.is_open())
  {
    throw std::runtime_error("Error reading matrix from file: " + filepath + ". File could not be opened.");
  }

  while (std::getline(file, line))
  {
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ','))
    {
      row.push_back(cell);
    }

    matrix.push_back(row);
  }

  file.close();
}
