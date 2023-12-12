#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "../../Config.hpp"
#include "AnimationReader.hpp"
#include "RehtiUtils.hpp"

std::vector<std::string> readAnimations()
{
    rapidjson::Document document = readJson(Config.ANIMATION_JSON_PATH);

    if (!document.IsArray())
    {
        throw std::runtime_error("Animation JSON file is not an array");
    }

    std::vector<std::string> animations;

    for (const auto& animation : document.GetArray())
    {
        if (!animation.IsString())
        {
            throw std::runtime_error("Animation JSON file contains non-string value");
        }

        animations.push_back(animation.GetString());
    }

    return animations;
}