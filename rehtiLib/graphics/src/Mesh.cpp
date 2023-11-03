#include "Mesh.hpp"



const std::vector<Vertex> TestValues::GetTestVertices()
{
	std::vector<Vertex> vertices{
	{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.f, 1.f) },
	{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f) },
	{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 0.f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 1.f) },
	{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 1.f) },
	{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 1.f, 1.f), glm::vec2(0.f, 0.f) },
	{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f), glm::vec2(1.f, 0.f) },
	{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.f, 0.f, 1.f), glm::vec2(1.f, 1.f) }
	};
	return vertices;
}

const std::vector<uint32_t> TestValues::GetTestIndices()
{
	return { // clockwise winding order
		2, 1, 0, 0, 3, 2,
			5, 6, 4, 6, 7, 4,
			0, 1, 5, 5, 4, 0,
			2, 3, 7, 2, 7, 6,
			1, 2, 6, 6, 5, 1,
			7, 3, 0, 7, 0, 4,
	};
}



const ImageData TestValues::GetTestTexture()
{
	unsigned char textureData[16 * 16] = {
	255, 0, 0, 255, 0, 255, 0, 255,
	0, 255, 0, 255, 0, 0, 255, 255,
	0, 0, 255, 255, 255, 0, 255, 255,
	255, 255, 0, 255, 255, 255, 0, 255,
	255, 0, 255, 255, 0, 255, 255, 255,
	0, 255, 255, 255, 255, 0, 255, 255,
	255, 255, 0, 255, 255, 255, 0, 255,
	255, 0, 255, 255, 0, 255, 255, 255,
	0, 255, 255, 255, 255, 0, 255, 255,
	255, 255, 0, 255, 255, 255, 0, 255,
	255, 0, 255, 255, 0, 255, 255, 255,
	0, 255, 255, 255, 255, 0, 255, 255,
	255, 255, 0, 255, 255, 255, 0, 255
	};

	ImageData img = {
		textureData,
		16,
		16,
	};
	return img;
}
