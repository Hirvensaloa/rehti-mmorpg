#include <glm/vec3.hpp>
#include "graphics.h"
#include "RehtiReader.hpp"

/**
 * @brief Convert aiVector3D to glm::vec3
 * @return Vertex
 */
Vertex aiVector3DToGlmVec3(const aiVector3D& vector)
{
	float texU = vector.x / Config.AREA_WIDTH;
	float texV = vector.z / Config.AREA_WIDTH;
	return Vertex{ glm::vec3(vector.x, vector.y, vector.z), glm::vec3(0.0f), glm::vec2(texU, texV) };
}

/**
 * @brief Convert aiFace to std::vector<unsigned int>
 * @return std::vector<unsigned int>
 */
std::vector<unsigned int> aiFaceToVector(const aiFace& face)
{
	std::vector<unsigned int> faceVector;
	for (unsigned int i = 0; i < face.mNumIndices; i++)
	{
		faceVector.push_back(face.mIndices[i]);
	}
	return faceVector;
}

/**
 * @brief Convert matrix of aiVector3D to std::vector<std::vector<Vertex>>
 * @param aiVector3DMatrix Matrix of aiVector3D
 * @return std::vector<std::vector<Vertex>>
 */
std::vector<std::vector<Vertex>> aiVector3DMatrixToVertexVector(const std::vector<std::vector<aiVector3D>>& aiVector3DMatrix)
{
	std::vector<std::vector<Vertex>> vertexVector;
	for (auto& row : aiVector3DMatrix)
	{
		std::vector<Vertex> vertexRow;
		for (auto& col : row)
		{
			vertexRow.push_back(aiVector3DToGlmVec3(col));
		}
		vertexVector.push_back(vertexRow);
	}
	return vertexVector;
}
/**
 * @brief Convert matrix of aiFace to std::vector<std::vector<unsigned int>>
 * @param aiFaceMatrix Matrix of aiFace
 * @return std::vector<std::vector<unsigned int>>
 */
std::vector<std::vector<uint32_t>> aiFaceMatrixToVector(const std::vector<std::vector<aiFace>>& aiFaceMatrix)
{
	std::vector<std::vector<unsigned int>> faceVector;
	for (auto& row : aiFaceMatrix)
	{
		std::vector<unsigned int> faceRow;
		for (auto& col : row)
		{
			std::vector<unsigned int> face = aiFaceToVector(col);
			faceRow.insert(faceRow.end(), face.begin(), face.end());
		}
		faceVector.push_back(faceRow);
	}
	return faceVector;
}