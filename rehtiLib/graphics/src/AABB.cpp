#include <math.h>

#include "AABB.hpp"
#include "RehtiUtils.hpp"

AABB::AABB(glm::vec3 min, glm::vec3 max, std::unique_ptr<AABB> pLeft, std::unique_ptr<AABB> pRight)
		: min(min), max(max), pLeft(std::move(pLeft)), pRight(std::move(pRight))
{
}

AABB::AABB(AABB &other)
{
	min = other.min;
	max = other.max;
	pLeft = std::move(other.pLeft);
	pRight = std::move(other.pRight);
}

AABB &AABB::operator=(AABB &other)
{
	min = other.min;
	max = other.max;
	pLeft = std::move(other.pLeft);
	pRight = std::move(other.pRight);
	return *this;
}

bool AABB::isLeaf() const
{
	return pLeft == nullptr && pRight == nullptr;
}

glm::vec3 AABB::getCenter() const
{
	return 0.5f * min + 0.5f * max;
}

glm::vec3 minVector(const glm::vec3 &a, const glm::vec3 &b)
{
	return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

glm::vec3 maxVector(const glm::vec3 &a, const glm::vec3 &b)
{
	return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

std::vector<std::unique_ptr<AABB>> createMapAABB(const MapAABBData &input)
{
	std::vector<std::unique_ptr<AABB>> aabbList;
	for (int y = 0; y < input.areaMatrix.size(); y++)
	{
		for (int x = 0; x < input.areaMatrix[y].size(); x++)
		{
			aabbList.push_back(createAreaAABB(nullptr, x * input.areaSize, y * input.areaSize, input.areaSize, input));
		}
	}
	return aabbList;
}

std::unique_ptr<AABB> createAreaAABB(AABB *aabb, const int x, const int y, const int size, const MapAABBData &input)
{
	if (size <= 1)
	{
		if (size != 1 || y >= input.heightMatrix.size() || x >= input.heightMatrix[y].size())
		{
			return nullptr;
		}

		TileHeight height = calculateTileHeights(input.heightMatrix, y, x);
		std::pair<float, float> minMax = findMinMaxHeight(height);
		const float minHeightScaled = minMax.first * input.heightScale;
		const float maxHeightScaled = minMax.second * input.heightScale;
		const float xScaled = x * input.sideScale;
		const float yScaled = y * input.sideScale;
		return std::make_unique<AABB>(glm::vec3(xScaled, minHeightScaled, yScaled), glm::vec3(xScaled + input.sideUnit * input.sideScale, maxHeightScaled, yScaled + input.sideUnit * input.sideScale), nullptr, nullptr);
	}
	else
	{
		std::unique_ptr<AABB> newAABB = std::make_unique<AABB>();
		const int split = floor(static_cast<float>(size / 2));
		newAABB->pLeft = createAreaAABB(aabb, x, y, split, input);
		newAABB->pRight = createAreaAABB(aabb, x + split, y + split, split, input);
		newAABB->min = minVector(newAABB->pLeft->min, newAABB->pRight->min);
		newAABB->max = maxVector(newAABB->pLeft->max, newAABB->pRight->max);
		return newAABB;
	}
}