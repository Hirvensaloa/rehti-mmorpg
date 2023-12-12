#include "RapidjsonHelpers.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

rapidjson::Document readJson(std::string path)
{
    std::ifstream jsonFile(path);
    if (!jsonFile.is_open())
    {
        throw std::runtime_error("Failed to open JSON file: " + path);
    }

    std::string jsonContent((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(jsonContent.c_str());

    if (doc.HasParseError())
    {
        throw std::runtime_error("Failed to parse JSON file: " + path);
    }

    return doc;
}

std::string valueTypeToString(ValueType type)
{
    switch (type)
    {
    case INT:
        return "int";
    case UINT:
        return "uint";
    case STRING:
        return "string";
    case BOOL:
        return "bool";
    case OBJECT:
        return "object";
    case ARRAY:
        return "array";
    default:
        return "unknown";
    }
};

bool validMember(const rapidjson::Value& value, const std::string& memberName, ValueType type, bool printError)
{
    if (!value.HasMember(memberName.c_str()))
    {
        if (printError)
        {
            std::cerr << "Error: Could not find member: " << memberName << std::endl;
        }
        return false;
    };

    bool ret = false;
    switch (type)
    {
    case INT:
        ret = value[memberName.c_str()].IsInt();
        break;
    case UINT:
        ret = value[memberName.c_str()].IsUint();
        break;
    case STRING:
        ret = value[memberName.c_str()].IsString();
        break;
    case BOOL:
        ret = value[memberName.c_str()].IsBool();
        break;
    case OBJECT:
        ret = value[memberName.c_str()].IsObject();
        break;
    case ARRAY:
        ret = value[memberName.c_str()].IsArray();
        break;
    }

    if (!ret && printError)
    {
        std::cerr << "Error: JSON item member type does not match. Key '" << memberName << "' should be " << valueTypeToString(type) << std::endl;
    }

    return ret;
};

rapidjson::Document createDocument()
{
    rapidjson::Document document;
    document.SetObject();

    return document;
}

/*
 * Parses a string into a rapidjson document.
 * Throws an exception if parsing fails.
 */
rapidjson::Document parseDocument(std::string str)
{
    rapidjson::Document document;
    document.Parse(str.c_str());

    if (document.HasParseError())
    {
        std::cout << "Error parsing json: " << document.GetParseError() << std::endl;
        throw std::runtime_error("Error parsing json");
    }

    return document;
}

/*
 * Creates a string from a rapidjson document.
 */
std::string createString(rapidjson::Document& document)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    const std::string str = buffer.GetString();
    return str;
}