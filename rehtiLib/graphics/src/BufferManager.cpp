#define VMA_IMPLEMENTATION
#include "BufferManager.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>


BufferManager::BufferManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue transferQueue, uint32_t queueFamily)
	: transferQueueM(transferQueue)
{
	// Create a command pool for the transfer queue
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = queueFamily;
	// Create the command pool and store it to the member variable
	if (vkCreateCommandPool(logDevice, &cmdPoolCreateInfo, nullptr, &transferCommandPoolM) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool for transfer queue");
	}

	// Create the allocator
	VmaAllocatorCreateInfo createInfo{};
	createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	createInfo.instance = instance;
	createInfo.device = logDevice;
	createInfo.physicalDevice = gpu;

	if (vmaCreateAllocator(&createInfo, &allocatorM) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA allocator");
	}

	// Add the queue family index
	queueFamilyIndicesM.push_back(queueFamily);
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

void BufferManager::addQueueFamilyAccess(const uint32_t queueFamily)
{
	queueFamilyIndicesM.push_back(queueFamily);
}

void BufferManager::createBuffer(VkDeviceSize size, const void* data, VkBufferUsageFlags usage, VmaMemoryUsage memUsage, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	AllocatedBuffer newBuffer{};

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT; // available to multiple queues
	bufferInfo.pQueueFamilyIndices = queueFamilyIndicesM.data();
	bufferInfo.queueFamilyIndexCount = queueFamilyIndicesM.size();	

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = memUsage;
	allocInfo.requiredFlags = requiredFlags;
	allocInfo.preferredFlags = preferredFlags;

	if (vmaCreateBuffer(allocatorM, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA buffer");
	}
	// Buffer creation succeeded, set the pointer to the data
	newBuffer.data = data;
	// Store the buffer and the allocation
	allocatedBuffersM.push_back(newBuffer);
}

void BufferManager::copyBuffers(VkDevice logDevice)
{
	std::vector<VkFence> fences;
	
	for (auto allocObject : this->allocatedBuffersM)
	{

		VkMemoryPropertyFlags memProps;
		vmaGetAllocationMemoryProperties(allocatorM, allocObject.allocation, &memProps);
		VkDeviceSize bufferSize = allocObject.allocation->GetSize();

		if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) // mappable memory
		{
			void* pDataMap;
			vmaMapMemory(allocatorM, allocObject.allocation, &pDataMap);
			memcpy(pDataMap, allocObject.data, bufferSize);
			// If the memory is not host coherent, we need to flush it
			if(! (memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) )
				vmaFlushAllocation(allocatorM, allocObject.allocation, 0, bufferSize);

			vmaUnmapMemory(allocatorM, allocObject.allocation);
		}
		else // non-mappable memory, needs to be staged and copied using cmd buffers
		{
			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			VmaAllocationInfo allocInfo;

			VkBufferCreateInfo stagingBufferInfo{};
			stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			stagingBufferInfo.size = bufferSize;
			stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			VmaAllocationCreateInfo stagingAllocInfo{};
			stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			stagingAllocInfo.requiredFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT 
				| VMA_ALLOCATION_CREATE_MAPPED_BIT; // Also automatically map

			if (vmaCreateBuffer(allocatorM, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingBufferAllocation, &allocInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create a staging buffer for a non-mappable buffer");
			}

			// Copy the data to the staging buffer
			memcpy(allocInfo.pMappedData, allocObject.data, bufferSize);
			vmaFlushAllocation(allocatorM, stagingBufferAllocation, 0, bufferSize);
			// Perhaps the following section should be separated into its own function
			// Allocate a command buffer
			VkCommandBufferAllocateInfo cmdAllocInfo{};
			cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmdAllocInfo.commandBufferCount = 1;
			cmdAllocInfo.commandPool = transferCommandPoolM; // TODO SET This, currently nullptr

			VkCommandBuffer cmdBuffer;
			if (vkAllocateCommandBuffers(logDevice, &cmdAllocInfo, &cmdBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate a command buffer for copying a non-mappable buffer");
			}

			// Begin recording
			VkCommandBufferBeginInfo cmdBeginInfo{};
			cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = bufferSize;
			vkCmdCopyBuffer(cmdBuffer, stagingBuffer, allocObject.buffer, 1, &copyRegion);

			vkEndCommandBuffer(cmdBuffer);


			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			VkFence fence;
			if (vkCreateFence(logDevice, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create a fence for copying a non-mappable buffer");
			}
			fences.push_back(fence);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdBuffer;

			vkQueueSubmit(transferQueueM, 1, &submitInfo, fence);
		} // end of else
	} // end of for

	if (!fences.empty()) // If we added fences, wait for them to finish
	{ 
		vkWaitForFences(logDevice, fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
		for (auto fence : fences)
		{
			vkDestroyFence(logDevice, fence, nullptr);
		}
	}
}
