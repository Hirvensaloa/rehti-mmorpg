#pragma once
#include "GraphicsTypes.hpp"

namespace TestValues
{
	// Simple cube mesh
	const std::vector<SimpleVertex> kSimpleCubeVertices
	{
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f)},
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 1.f, 0.f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.f, 1.f, 0.f)},
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f)},
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f)},
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 1.f, 1.f)},
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 1.f)},
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.f, 0.f, 1.f)}
	};


	const std::vector<uint32_t> kSimpleCubeIndices
	{ // clockwise winding order
		2, 1, 0, 0, 3, 2,
		5, 6, 4, 6, 7, 4,
		0, 1, 5, 5, 4, 0,
		2, 3, 7, 2, 7, 6,
		1, 2, 6, 6, 5, 1,
		7, 3, 0, 7, 0, 4,
	};

	const std::vector<Vertex> GetTestVertices();

	const std::vector<uint32_t> GetTestIndices();

	const ImageData GetTestTexture();
}

class SimpleMesh
{
public:

private:
	std::vector<SimpleVertex> verticesM;
	std::vector<uint32_t> indicesM;
};

