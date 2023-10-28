#pragma once
#include "DescriptorBuilder.hpp"
#include "Mesh.hpp"

#include <vk_mem_alloc.h>
#include <vector>
#include <unordered_map>
#include <array>

typedef enum
{
	CHARACTER,
	GAMEOBJECT,
	TESTOBJECT
} ObjectType;

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
	VkDescriptorSet descriptorSet; // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	AllocatedImage texture; // Texture of the object (perhaps combined imagesampler later)
	void * mappedTransformData; // Pointer to the mapped data of the transform buffer
	void * mappedTextureData; // Pointer to the mapped data of the texture
};

// Test object descriptor data
struct TestObjectUniformBuffer
{
	VkDescriptorSet descriptorSet; // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	void * mappedTransformData; // Pointer to the mapped data of the transform buffer
};

// Character buffer object
struct CharacterObjectUniformBuffer
{
	VkDescriptorSet descriptorSet; // Descriptor set of the data
	AllocatedBuffer boneTransformations; // Todo think how the data is going to be stored
	AllocatedBuffer boneWeights;
};

// Object that contains everything needed to render a character.
struct CharacterObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	std::vector<CharacterObjectUniformBuffer> characterUniformBuffers;
	inline static std::array<VkDescriptorSetLayoutBinding, 2> getDescriptorSetLayoutBindings();
};

struct GameObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	std::vector<GameObjectUniformBuffer> uniformBuffers;
	inline static std::array<VkDescriptorSetLayoutBinding, 2> getDescriptorSetLayoutBindings();
};

struct TestObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	std::vector<TestObjectUniformBuffer> uniformBuffers;
	inline static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

class GraphicsObjectManager
{
public:
	/// <summary>
	/// Creates a graphics object manager.
	/// </summary>
	/// <param name="instance"> is a vulkan instance.</param>
	/// <param name="gpu"></param>
	/// <param name="logDevice"></param>
	/// <param name="transferQueue"> that can receive transfer operations.</param>
	/// <param name="queueFamily"> of the queue that was supplied</param>
	GraphicsObjectManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue transferQueue, uint32_t queueFamily, const uint32_t frameCount);
	~GraphicsObjectManager();

	/// <summary>
	/// Adds access to the given queue families to the buffer manager.
	/// </summary>
	/// <param name="families"></param>
	/// <param name="familyCount"></param>
	void addQueueFamilyAccess(const uint32_t queueFamily);

	/// <summary>
	/// Adds a character to the buffer manager.
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	bool addCharacter(int id);

	/// <summary>
	/// Creates a test object with the given parameters.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	/// <param name="transformation"></param>
	/// <returns></returns>
	bool addTestObject(int id, const std::vector<SimpleVertex>& vertices, const std::vector<uint32_t>& indices, glm::mat4 transformation);

	/// <summary>
	/// Updates test object data
	/// </summary>
	/// <param name="id"></param>
	/// <param name="srcData"></param>
	/// <param name="frame"></param>
	void updateTestObject(int id, const void* srcData, uint32_t frame);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="id"></param>
	/// <param name="srcData"></param>
	/// <param name="frame"></param>
	void updateObjectDescriptor(int id, const void* srcData, uint32_t frame);

	/// <summary>
	/// Copies given buffers to the gpu. Not very useful function currently.
	/// </summary>
	/// <param name="logDevice"></param>
	/// <param name="sources"></param>
	void copyBuffers(VkDevice logDevice, std::vector<AllocatedBuffer> buffers, std::vector<const void*> sources);

	AllocatedImage createDepthImage();

	/// <summary>
	/// Copies the given data to the given buffer.
	/// </summary>
	/// <param name="logDevice"> is the logical device</param>
	/// <param name="allocBuffer"> is the allocated buffer struct</param>
	/// <param name="srcData"> is the data to cpy</param>
	void copyBuffer(VkDevice logDevice, AllocatedBuffer allocBuffer, const void* srcData);


	/// <summary>
	/// Returns the layout of the given type.
	/// </summary>
	/// <param name="type"> of object for which a layout is desired</param>
	/// <returns>Descriptor set layout</returns>
	VkDescriptorSetLayout getLayout(ObjectType type) const;

	/// <summary>
	/// Returns drawable objects of the given type.
	/// </summary>
	/// <param name="type"> of object desired for drawing</param>
	/// <returns>List of drawableobject structs</returns>
	std::vector<DrawableObject> getDrawableObjects(ObjectType type, uint32_t frame) const;

private:

	/// <summary>
	/// Creates a buffer with the given parameters.
	/// </summary>
	/// <param name="size">Size of the buffer in bytes.</param>
	/// <param name="usage">Flags signaling the usage of the buffer to be allocated.</param>
	AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationInfo& info, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);
	AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

	/// <summary>
	/// Creates image
	/// </summary>
	/// <param name="size"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="format"></param>
	/// <param name="tiling"></param>
	/// <param name="usage"></param>
	/// <param name="memUsage"></param>
	/// <param name="vmaCreationFlags"></param>
	/// <param name="requiredFlags"></param>
	/// <param name="preferredFlags"></param>
	/// <returns></returns>
	AllocatedImage createImage(VkDeviceSize size, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);
	void createDescriptorBuilder();

	VkDevice logDeviceM;
	VmaAllocator allocatorM;
	VkQueue transferQueueM;
	VkCommandPool transferCommandPoolM;
	std::unique_ptr<DescriptorBuilder> pBuilderM;
	std::unordered_map<int, CharacterObject> characterObjectsM;
	std::unordered_map<int, GameObject> gameObjectsM;
	std::unordered_map<int, TestObject> testObjectsM;
	std::vector<uint32_t> queueFamilyIndicesM;
	uint32_t frameCountM;
	VkDescriptorSetLayout characterSetLayoutM;
	VkDescriptorSetLayout gameObjectSetLayoutM;
	VkDescriptorSetLayout testObjectSetLayoutM;
};
