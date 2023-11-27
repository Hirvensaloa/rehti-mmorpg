#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <vector>

#include "RehtiReader.hpp"

/**
 * @brief Generate GeneratedAnimations.hpp file
 */
void generateAnimationHppFile()
{
    std::vector<std::string> animationNames = readAnimations();

    std::ofstream hppFile(Config.GENERATED_ANIMATIONS_HPP_PATH);

    if (hppFile.is_open())
    {
        hppFile << "/**\n";
        hppFile << " * WARNING: This file is auto-generated by a script. Do not modify manually.\n";
        hppFile << " */\n\n";

        hppFile << "#pragma once\n\n";

        hppFile << "#include <cstdint>\n";
        hppFile << "#include <algorithm>\n";
        hppFile << "#include <string>\n\n";

        hppFile << "constexpr size_t ANIMATION_TYPE_COUNT = " << animationNames.size() << ";\n\n";
        hppFile << "enum AnimationType : uint32_t\n";
        hppFile << "{\n";

        for (size_t i = 0; i < animationNames.size(); ++i)
        {
            std::string enumName = animationNames[i];
            std::transform(enumName.begin(), enumName.end(), enumName.begin(), ::toupper);

            hppFile << "    " << enumName;
            if (i < animationNames.size() - 1)
            {
                hppFile << ",";
            }
            hppFile << "\n";
        }

        hppFile << "};\n\n";
        hppFile << "inline const char* AnimationTypeStrings[] = {\n";

        for (size_t i = 0; i < animationNames.size(); ++i)
        {
            std::string lowercaseName = animationNames[i];
            std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);

            hppFile << "    \"" << lowercaseName << "\"";
            if (i < animationNames.size() - 1)
            {
                hppFile << ",";
            }
            hppFile << "\n";
        }

        hppFile << "};\n\n";

        // Generate getAnimationType function
        hppFile << "inline AnimationType getAnimationType(const std::string animationName) {\n";
        hppFile << "    std::string lowercaseName(animationName);\n";
        hppFile << "    std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);\n\n";
        hppFile << "    for (size_t i = 0; i < ANIMATION_TYPE_COUNT; ++i) {\n";
        hppFile << "        if (lowercaseName.find(AnimationTypeStrings[i]) != std::string::npos) {\n";
        hppFile << "            return static_cast<AnimationType>(i);\n";
        hppFile << "        }\n";
        hppFile << "    }\n\n";
        hppFile << "    // Default to the first animation type if no match is found\n";
        hppFile << "    return static_cast<AnimationType>(0);\n";
        hppFile << "}\n";

        hppFile.close();
        std::cout << "GeneratedAnimations.hpp generated successfully.\n";
    }
    else
    {
        std::cerr << "Unable to open GeneratedAnimations.hpp for writing.\n";
    }
}