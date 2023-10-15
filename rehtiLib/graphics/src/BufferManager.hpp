#pragma once
#include <vk_mem_alloc.h>
#include <vector>

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
	const void * data;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	const void * data;
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
	/// Creates a buffer with the given parameters, and stores the created buffer and the corresponding allocation to a vector.
	/// // Todo : Add mesh as a parameter, so that creation for its memory is abstracted further.
	/// </summary>
	/// <param name="size">Size of the buffer in bytes.</param>
	/// <param name="usage">Flags signaling the usage of the buffer to be allocated.</param>
	void createBuffer(VkDeviceSize size, const void * data, VkBufferUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

	// Copies buffers to the gpu
	void copyBuffers(VkDevice logDevice);

private:

	VmaAllocator allocatorM;
	VkQueue transferQueueM; // Todo set
	VkCommandPool transferCommandPoolM; // Todo set
	std::vector<AllocatedBuffer> allocatedBuffersM;
	std::vector<AllocatedImage> allocatedImagesM;
	std::vector<uint32_t> queueFamilyIndicesM;

};
