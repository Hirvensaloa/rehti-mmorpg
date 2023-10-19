#pragma once
#include "SimpleMesh.hpp"

#include <vk_mem_alloc.h>
#include <vector>

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
	void * data;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	void * data;
};

class DrawAbleObject
{
public:
	DrawAbleObject(AllocatedBuffer vBuffer, AllocatedBuffer iBuffer, std::vector<AllocatedBuffer> uBuffers, uint32_t iCount, glm::mat4 initTransform);
	~DrawAbleObject();
	VkBuffer getVertexBuffer() { return vertexBufferM.buffer; }
	VkBuffer getIndexBuffer() { return indexBufferM.buffer; }
	VkBuffer getUniformBuffer(uint32_t index) { return uniformBuffersM[index].buffer; }
	VmaAllocation getVertexBufferAllocation() { return vertexBufferM.allocation; }
	VmaAllocation getIndexBufferAllocation() { return indexBufferM.allocation; }
	void applyTransformation(uint32_t index, glm::mat4 transform);
	void setTransformation(uint32_t index, glm::mat4 newTransform);
	void update(uint32_t index);
private:
	AllocatedBuffer vertexBufferM;
	AllocatedBuffer indexBufferM;
	uint32_t indexCountM;
	std::vector<AllocatedBuffer> uniformBuffersM;
	glm::mat4 modelToWorldMatrixM;
};

class BufferManager
{
public:
	// TODO: Constructors may be a bad idea, since the allocator needs to be initialized with the instance, device and gpu
	// This means that we have no quarantee that the instance, device and gpu are valid at the time of destruction
	BufferManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue transferQueue, uint32_t queueFamily);
	~BufferManager();

	/// <summary>
	/// Adds access to the given queue families to the buffer manager.
	/// </summary>
	/// <param name="families"></param>
	/// <param name="familyCount"></param>
	void addQueueFamilyAccess(const uint32_t queueFamily);


	/// <summary>
	/// Creates a drawable object with the given parameters, and stores it to a vector.
	/// </summary>
	/// <param name="concurrentFrames">Number of concurrent frames in the application</param>
	/// <param name="vertices">Vertices of the object</param>
	/// <param name="indices">Indices of the object</param>
	/// <param name="startingTransformation">Optional starting transformation for the object.</param>
	void createDrawableObject(const uint32_t concurrentFrames, const std::vector<SimpleVertex>& vertices, const std::vector<uint32_t>& indices, glm::mat4 startingTransformation = glm::mat4(1.f));

	// Copies buffers to the gpu
	void copyBuffers(VkDevice logDevice);

private:

	/// <summary>
	/// Creates a buffer with the given parameters, and stores the created buffer and the corresponding allocation to a vector.
	/// // Todo : Add mesh as a parameter, so that creation for its memory is abstracted further.
	/// </summary>
	/// <param name="size">Size of the buffer in bytes.</param>
	/// <param name="usage">Flags signaling the usage of the buffer to be allocated.</param>
	AllocatedBuffer createBuffer(VkDeviceSize size, void* data, VkBufferUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

	VmaAllocator allocatorM;
	VkQueue transferQueueM;
	VkCommandPool transferCommandPoolM;
	std::vector<DrawAbleObject> drawAbleObjectsM;
	std::vector<uint32_t> queueFamilyIndicesM;

};
