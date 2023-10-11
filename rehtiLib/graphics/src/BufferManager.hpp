#pragma once
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <vector>

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

class BufferManager
{
public:
	// TODO: Constructors may be a bad idea, since the allocator needs to be initialized with the instance, device and gpu
	// This means that we have no quarantee that the instance, device and gpu are valid at the time of destruction
	BufferManager(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu);
	~BufferManager();

	/// <summary>
	/// Creates a buffer with the given parameters, and stores the created buffer and the corresponding allocation to a vector.
	/// </summary>
	/// <param name="size">Size of the buffer in bytes.</param>
	/// <param name="usage">Flags signaling the usage of the buffer to be allocated.</param>
	/// <param name="properties">Desired memory property flags of the buffer.</param>
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VmaMemoryUsage memUsage=VMA_MEMORY_USAGE_AUTO);

private:
	VmaAllocator allocatorM;
	std::vector<AllocatedBuffer> allocatedBuffersM;
};
