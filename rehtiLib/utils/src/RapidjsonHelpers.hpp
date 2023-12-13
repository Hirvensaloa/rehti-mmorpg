#pragma once

#include <string>

#include "rapidjson/document.h"

/**
 * @file contains helper functions for rapidjson-library
 */

/**
 * @brief Reads JSON file to rapidjson document
 *
 * @param path to JSON file
 * @return rapidjson::Document
 */
rapidjson::Document readJson(std::string path);

/**
 * @brief Rapidjson value types (not exhaustive)
 */
enum ValueType
{
    INT,
    UINT,
    STRING,
    BOOL,
    OBJECT,
    ARRAY
};

/**
 * @brief Convertes ValueType to string
 *
 * @param type
 * @return std::string
 */
std::string valueTypeToString(ValueType type);

/**
 * @brief Check if rapidjson value contains a member with the given name AND check that the type matches the given type
 *
 * @param value rapidjson value
 * @param memberName
 * @param type ValueType
 * @param printError If true, will print an error message if the member is not found
 * @return true if the member exists and the type matches, false otherwise
 */
bool validMember(const rapidjson::Value& value, const std::string& memberName, ValueType type, bool printError = true);

/**
 * @brief Creates rapidjson document *
 * @return rapidjson::Document
 */
rapidjson::Document createDocument();

/**
 * @brief Parses a string into a rapidjson document.
 * @note Throws an exception if parsing fails.
 * @return rapidjson::Document
 */
rapidjson::Document parseDocument(std::string str);

/**
 * @brief Creates a string from a rapidjson document.
 * @return std::string
 */
std::string createString(rapidjson::Document& document);