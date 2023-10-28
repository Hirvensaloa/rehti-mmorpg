#pragma once

#include <vector>

const std::string ROOT_PATH = "./";
const std::string GENERATED_ASSETS_PATH = ROOT_PATH + "assets/generated/";

/*
 * Reads a matrix from a text file. Values should be separated by commas and rows by newlines.
 *
 * Row sizes do not need to match.
 */
void readMatrixFromFile(std::vector<std::vector<std::string>> &matrix, std::string filepath);