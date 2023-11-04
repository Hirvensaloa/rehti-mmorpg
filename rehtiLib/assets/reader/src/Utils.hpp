#pragma once

#include <vector>

const std::string ROOT_PATH = "./";
const std::string GENERATED_ASSETS_PATH = ROOT_PATH + "assets/generated/";

/**
 * @brief Reads a matrix from a text file. Values should be separated by commas and rows by newlines.
 *
 * @param matrix Empty matrix of strings that will be filled with the matrix information.
 * @param filepath Path to the file.
 *
 * @returns Nothing, but fills the matrix.
 */
void readMatrixFromFile(std::vector<std::vector<std::string>> &matrix, std::string filepath);