
#include "BufferManager.hpp"

BufferManager::BufferManager(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu)
{
	VmaAllocatorCreateInfo createInfo{};
	createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	createInfo.instance = instance;
	createInfo.device = logDevice;
	createInfo.physicalDevice = gpu;

	if (vmaCreateAllocator(&createInfo, &allocatorM) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA allocator");
	}
}

BufferManager::~BufferManager()
{
	// Loop over allocated buffers and destroy them
	for (auto& buffer : allocatedBuffersM)
	{
		vmaDestroyBuffer(allocatorM, buffer.buffer, buffer.allocation);
	}
	// And destroy the allocator
	vmaDestroyAllocator(allocatorM);
}

void BufferManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO)
{
	AllocatedBuffer newBuffer{};

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = memUsage;

	if (vmaCreateBuffer(allocatorM, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA buffer");
	}
	// Store the buffer and the allocation
	allocatedBuffersM.push_back(newBuffer);

}
