#pragma once

#include "BasicTypes.hpp"
#include <vk_mem_alloc.h>

struct DrawableObject
{
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
	VkDescriptorSet descriptorSet;
};

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
	VkDeviceSize size;
};


struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VkDeviceSize size;
};

// Game object descriptor data
struct GameObjectUniformBuffer
{
	VkDescriptorSet descriptorSet;   // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	void* mappedTransformData;       // Pointer to the mapped data of the transform buffer
};

// Test object descriptor data
struct TestObjectUniformBuffer
{
	VkDescriptorSet descriptorSet;   // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	void* mappedTransformData;       // Pointer to the mapped data of the transform buffer
};

// Character buffer object
struct CharacterObjectUniformBuffer
{
	VkDescriptorSet descriptorSet;       // Descriptor set of the data
	AllocatedBuffer transformBuffer;     // Buffer containing model to world matrix
	AllocatedBuffer boneTransformations; // Buffer containing bone transformations
};

// Object that contains everything needed to render a character.
struct CharacterObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	VkImageView textureView;
	std::vector<CharacterObjectUniformBuffer> characterUniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 3> getDescriptorSetLayoutBindings();
};

struct GameObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	VkImageView textureView;
	std::vector<GameObjectUniformBuffer> uniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 2> getDescriptorSetLayoutBindings();
};

struct AreaObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	std::array<AllocatedImage, 6> textures; // 6: blendMap, base and r, g, b, a
	std::array<VkImageView, 6> textureViews;
	VkDescriptorSet descriptorSet;
	static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

struct TestObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	std::vector<TestObjectUniformBuffer> uniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

#endif

#pragma endregion

#pragma region VertexTypes


#pragma endregion