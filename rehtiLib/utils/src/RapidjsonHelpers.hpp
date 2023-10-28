#pragma once

#include "rapidjson/document.h"

// Reads JSON file to rapidjson document
rapidjson::Document readJson(std::string path);

enum ValueType
{
  INT,
  UINT,
  STRING,
  BOOL,
  OBJECT,
  ARRAY
};

std::string valueTypeToString(ValueType type);

// Check if rapid json value contains a member with the given name AND check that the type matches the given type
bool validMember(const rapidjson::Value &value, const std::string &memberName, ValueType type, bool required = true);

rapidjson::Document createDocument();

/*
 * Parses a string into a rapidjson document.
 * Throws an exception if parsing fails.
 */
rapidjson::Document parseDocument(std::string str);

/*
 * Creates a string from a rapidjson document.
 */
std::string createString(rapidjson::Document &document);