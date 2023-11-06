#include <filesystem>
#include <vector>
#include <iostream>

#include "TextureReader.hpp"
#include "../../Config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

std::map<std::string, StbImageData> loadTextures()
{
	std::map<std::string, StbImageData> textures;

	int width, height, channels;
	unsigned char* data;

	// Read all the texture filenames
	std::vector<std::string> textureFilenames;
	for (const auto& entry : std::filesystem::directory_iterator(Config.TEXTURE_PATH))
	{
		textureFilenames.push_back(entry.path().filename().string());
	}

	// Read all the textures
	for (const auto& filename : textureFilenames)
	{
		data = stbi_load((Config.TEXTURE_PATH + filename).c_str(), &width, &height, &channels, 4);
		if (data)
		{
			textures[filename] = StbImageData{ data, width, height };
			std::cout << "Loaded texture: " << filename << std::endl;
		}
		else
		{
			std::cout << "Failed to load texture: " << filename << std::endl;
		}
	}

	return textures;
}