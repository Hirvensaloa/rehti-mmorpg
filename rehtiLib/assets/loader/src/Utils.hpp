#pragma once

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <vector>

#include "../lodepng/lodepng.h"

/**
 * @brief Reads all the files in the directory and returns a vector of the filenames. Only returns files with the given extension.
 *
 * @param path Path to the directory
 * @param extension File extension to filter the files
 * @return std::vector<std::string> Vector of filenames
 */
std::vector<std::string> readDirectory(std::string path, std::string extension)
{
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == extension)
        {
            files.push_back(entry.path().filename().string());
        }
    }

    return files;
}

/**
 * @brief Reads a PNG file and returns the image data, width and height.
 *
 * @param image Vector to store the image data
 * @param width Width of the image
 * @param height Height of the image
 * @param filepath Filepath to the PNG file
 */
void readPng(std::vector<unsigned char>& image, unsigned& width, unsigned& height, std::string filepath)
{
    std::cout << "Reading " << filepath << std::endl;

    unsigned error = lodepng::decode(image, width, height, filepath);

    if (error)
    {
        throw std::runtime_error("PNG decoding error" + std::to_string(error) + ": " + lodepng_error_text(error));
    }
}

/**
 * @brief Rotates the matrix 90 degrees counter-clockwise. Rotation is calculated by rotation-param * 90 degrees.
 *
 * @param matrix Matrix to rotate
 * @param rotation Number of 90 degree rotations
 */
template <typename T>
void rotateMatrix(std::vector<std::vector<T>>& matrix, uint8_t rotation)
{
    if (rotation == 0)
    {
        return;
    }

    std::vector<std::vector<T>> rotatedMatrix;

    for (unsigned i = 0; i < matrix[0].size(); i++)
    {
        std::vector<T> row;
        for (int j = matrix.size() - 1; j >= 0; j--)
        {
            row.push_back(matrix[j][i]);
        }
        rotatedMatrix.push_back(row);
    }

    matrix = rotatedMatrix;

    rotateMatrix(matrix, rotation - 1);
}

/**
 * @brief Populates a matrix with a default value. The matrix is all the areas combined and expanded e.g. the whole map.
 * The matrix row sizes DO NOT need to match.
 *
 * @param matrix
 * @param areaMap
 * @param defaultValue
 * @param AREA_WIDTH
 * @param AREA_HEIGHT
 */
template <typename T>
void populateMatrix(std::vector<std::vector<T>>& matrix, const std::vector<std::vector<std::string>>& areaMap, T defaultValue, unsigned AREA_WIDTH, unsigned AREA_HEIGHT)
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

/**
 * @brief Populates a matrix from a reference matrix. Uses a default value for empty cells. Reference matrix is given (instead of it's dimensions) because the row sizes might not match.
 *
 * @param matrix
 * @param referenceMatrix
 * @param defaultValue to fill the matrix with
 */
template <typename T, typename K>
void populateMatrixFromReference(std::vector<std::vector<T>>& matrix, const std::vector<std::vector<K>>& referenceMatrix, T defaultValue)
{
    for (unsigned i = 0; i < referenceMatrix.size(); i++)
    {
        std::vector<T> row(referenceMatrix[i].size(), defaultValue);
        matrix.push_back(row);
    }
}

/**
 * @brief Takes in a matrix<T> and writes that to a text file. Values are separated by commas and rows by newlines.
 *
 * Row sizes do not need to match.
 * @param matrix Matrix to write to file
 * @param filepath Filepath to write to
 */
template <typename T>
void writeMatrixToFile(const std::vector<std::vector<T>>& matrix, const std::string& filepath)
{
    // Create the directory structure if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());

    std::ofstream file(filepath, std::ios::out); // std::ios::out to create the file if it does not exist

    if (file.is_open())
    {
        for (const auto& row : matrix)
        {
            for (const auto& col : row)
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
