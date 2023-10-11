#pragma once

#include <glm/vec3.hpp>

#include <vector>

// TODO Use assimp and checkout the library.
struct SimpleVertex
{
	glm::vec3 pos;
	glm::vec3 color;
};

// Simple cube mesh
const std::vector<SimpleVertex> kSimpleCube
{
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)},
	{ glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
	{ glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 0.f)},
	{ glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
	{ glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f)},
	{ glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 1.f)},
	{ glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f)},
	{ glm::vec3(1.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 1.f)}
};

const std::vector<uint32_t> kSimpleCubeIndices
{ // counterclockwise winding order
	2, 1, 0, 0, 3, 2,
	4, 5, 6, 6, 7, 4,
	0, 1, 5, 5, 4, 0,
	2, 3, 7, 2, 7, 6,
	1, 2, 6, 6, 5, 1,
	7, 3, 0, 7, 0, 5,
};

class SimpleMesh
{
public:


private:
	std::vector<SimpleVertex> verticesM;
	std::vector<uint32_t> indicesM;
};

