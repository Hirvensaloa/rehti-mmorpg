#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
#include <filesystem>
#include <exception>

#include "rapidjson/document.h"
#include "../lodepng/lodepng.h"

// Reads all the files in the directory and returns a vector of the filenames. Only returns files with the given extension.
std::vector<std::string> readDirectory(std::string path, std::string extension)
{
  std::vector<std::string> files;

  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (entry.path().extension() == extension)
    {
      files.push_back(entry.path().filename());
    }
  }

  return files;
}

// Reads JSON file to rapidjson document
rapidjson::Document readJson(std::string path)
{
  std::ifstream jsonFile(path);
  if (!jsonFile.is_open())
  {
    throw std::runtime_error("Failed to open JSON file");
  }

  std::string jsonContent((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());

  rapidjson::Document doc;
  doc.Parse(jsonContent.c_str());

  if (doc.HasParseError())
  {
    throw std::runtime_error("Failed to parse JSON file");
  }

  return doc;
}

void readPng(std::vector<unsigned char> &image, unsigned &width, unsigned &height, std::string filepath)
{
  std::cout << "Reading " << filepath << std::endl;

  unsigned error = lodepng::decode(image, width, height, filepath);

  if (error)
  {
    throw std::runtime_error("PNG decoding error" + std::to_string(error) + ": " + lodepng_error_text(error));
  }
}

// Rotates the matrix 90 degrees clockwise. Rotation is done rotation * 90 degrees.
template <typename T>
void rotateMatrix(std::vector<std::vector<T>> &matrix, uint rotation)
{
  int maxRotation = 4;
  rotation = rotation % maxRotation;

  int rows = matrix.size();
  int cols = matrix[0].size();

  for (int r = 0; r < rotation; r++)
  {
    std::vector<std::vector<T>> rotatedMatrix(cols, std::vector<T>(rows));

    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        int newRow = j;
        int newCol = rows - 1 - i;

        rotatedMatrix[newRow][newCol] = matrix[i][j];
      }
    }

    matrix = rotatedMatrix;
    std::swap(rows, cols); // Update rows and cols for the next rotation
  }
}

/*
 * Populates a matrix with a default value. The matrix is all the areas combined and expanded e.g. the whole map.
 * The matrix row sizes DO NOT need to match.
 */
template <typename T>
void populateMatrix(std::vector<std::vector<T>> &matrix, const std::vector<std::vector<std::string>> &areaMap, T defaultValue, unsigned AREA_WIDTH, unsigned AREA_HEIGHT)
{
  for (unsigned i = 0; i < areaMap.size(); i++)
  {
    for (unsigned j = 0; j < AREA_HEIGHT; j++)
    {
      std::vector<T> row(AREA_WIDTH * areaMap[i].size(), defaultValue);
      matrix.push_back(row);
    }
  }
}

// Populates a matrix from a reference matrix. Uses a default value for empty cells. Reference matrix is given (instead of it's dimensions) because the row sizes might not match.
template <typename T, typename K>
void populateMatrixFromReference(std::vector<std::vector<T>> &matrix, const std::vector<std::vector<K>> &referenceMatrix, T defaultValue)
{
  for (unsigned i = 0; i < referenceMatrix.size(); i++)
  {
    std::vector<T> row(referenceMatrix[i].size(), defaultValue);
    matrix.push_back(row);
  }
}

/*
 * Takes in a matrix<T> and writes that to a text file. Values are separated by commas and rows by newlines.
 *
 * Row sizes do not need to match.
 */
template <typename T>
void writeMatrixToFile(const std::vector<std::vector<T>> &matrix, const std::string &filepath)
{
  // Create the directory structure if it doesn't exist
  std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());

  std::ofstream file(filepath, std::ios::out); // std::ios::out to create the file if it does not exist

  if (file.is_open())
  {
    for (const auto &row : matrix)
    {
      for (const auto &col : row)
      {
        file << col << ',';
      }
      file << '\n';
    }

    file.close();
  }
  else
  {
    throw std::runtime_error("Unable to open the file for writing.");
  }
}
