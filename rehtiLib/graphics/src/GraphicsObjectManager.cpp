#define VMA_IMPLEMENTATION
#include "GraphicsObjectManager.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>

#pragma region objects

std::array<VkDescriptorSetLayoutBinding, 1> TestObject::getDescriptorSetLayoutBindings()
{
	std::array<VkDescriptorSetLayoutBinding, 1> bindings = {};
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr;
	return bindings;
}

#pragma endregion

#pragma region Manager 

GraphicsObjectManager::GraphicsObjectManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue transferQueue, uint32_t queueFamily, const uint32_t frameCount)
	: transferQueueM(transferQueue), logDeviceM(logDevice), frameCountM(frameCount)
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

	// Create layouts for all object types
	createDescriptorBuilder();
	auto bindings = TestObject::getDescriptorSetLayoutBindings();
	pBuilderM->setDescriptorSetLayout(bindings.data(), bindings.size(), testObjectSetLayoutM);
	// Todo add other objects
}


GraphicsObjectManager::~GraphicsObjectManager()
{
	// Loop over allocated character objects and destroy them

	for (auto& characterPair : characterObjectsM)
	{
		auto character = characterPair.second;
		vmaDestroyBuffer(allocatorM, character.vertexData.buffer, character.vertexData.allocation);
		vmaDestroyBuffer(allocatorM, character.indexData.buffer, character.indexData.allocation);
		vmaDestroyImage(allocatorM, character.texture.image, character.texture.allocation);
		// Todo destroy skeleton data
	}
	// Loop over objects
	for (auto& objectPair : gameObjectsM)
	{
		auto object = objectPair.second;
		vmaDestroyBuffer(allocatorM, object.vertexData.buffer, object.vertexData.allocation);
		vmaDestroyBuffer(allocatorM, object.indexData.buffer, object.indexData.allocation);
		vmaDestroyImage(allocatorM, object.texture.image, object.texture.allocation);
	}
	// test objects
	for (auto& testPair : testObjectsM)
	{
		auto test = testPair.second;
		vmaDestroyBuffer(allocatorM, test.vertexData.buffer, test.vertexData.allocation);
		vmaDestroyBuffer(allocatorM, test.indexData.buffer, test.indexData.allocation);
		for (auto& bufObject : test.uniformBuffers)
		{
			vmaDestroyBuffer(allocatorM, bufObject.transformBuffer.buffer, bufObject.transformBuffer.allocation);
		}
	}

	vkDestroyCommandPool(logDeviceM, transferCommandPoolM, nullptr);
	// And destroy the allocator
	vmaDestroyAllocator(allocatorM);
}

void GraphicsObjectManager::addQueueFamilyAccess(const uint32_t queueFamily)
{
	if (std::find(queueFamilyIndicesM.begin(), queueFamilyIndicesM.end(), queueFamily) == queueFamilyIndicesM.end())
	{
		queueFamilyIndicesM.push_back(queueFamily);
	}
}

AllocatedBuffer GraphicsObjectManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationInfo& info, VmaMemoryUsage memUsage, VmaAllocationCreateFlags vmaCreationFlags, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
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
	allocInfo.flags = vmaCreationFlags;
	allocInfo.usage = memUsage;
	allocInfo.requiredFlags = requiredFlags;
	allocInfo.preferredFlags = preferredFlags;

	if (vmaCreateBuffer(allocatorM, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, &info) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA buffer");
	}
	newBuffer.size = size;

	// Store the buffer and the allocation
	return newBuffer;
}

AllocatedBuffer GraphicsObjectManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage, VmaAllocationCreateFlags vmaCreationFlags, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
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
	allocInfo.flags = vmaCreationFlags;
	allocInfo.usage = memUsage;
	allocInfo.requiredFlags = requiredFlags;
	allocInfo.preferredFlags = preferredFlags;	

	if (vmaCreateBuffer(allocatorM, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VMA buffer");		
	}
	newBuffer.size = size;

	// Store the buffer and the allocation
	return newBuffer;
}

AllocatedImage GraphicsObjectManager::createImage(VkDeviceSize size, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage, VmaAllocationCreateFlags vmaCreationFlags, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	AllocatedImage newImage{};

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;


	return AllocatedImage();
}

void GraphicsObjectManager::createDescriptorBuilder()
{
	PoolManager manager(logDeviceM);
	DescriptorSetLayoutCache cache(logDeviceM);
	pBuilderM = std::make_unique<DescriptorBuilder>(manager, cache);
}

bool GraphicsObjectManager::addTestObject(int id, const std::vector<SimpleVertex>& vertices, const std::vector<uint32_t>& indices, glm::mat4 transformation)
{
	if (testObjectsM.contains(id))
	{
		return false;
	}	

	// Create the vertex buffer
	TestObject newObject{};
	newObject.indexCount = indices.size();
	VkDeviceSize vertexBufferSize = vertices.size() * sizeof(SimpleVertex);
	VkDeviceSize indexBufferSize = indices.size() * sizeof(uint32_t);
	VkDeviceSize transformBufferSize = sizeof(glm::mat4);

	// create Vertex and index buffers
	newObject.vertexData = createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	newObject.indexData = createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	// Create descriptor set data
	for (uint32_t i = 0; i < frameCountM; i++)
	{
		TestObjectUniformBuffer uBuffer{};
		// We want uniform memory to be host coherent, and persistently mapped
		uBuffer.transformBuffer = createBuffer(transformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT); // These two flags set memory to be persistently mapped.
		VkDescriptorBufferInfo descBufferInfo{};
		descBufferInfo.buffer = uBuffer.transformBuffer.buffer;
		descBufferInfo.offset = 0;
		descBufferInfo.range = transformBufferSize;
		// Try creating descriptor set
		if (pBuilderM->bindBuffer(descBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).build(uBuffer.descriptorSet))
		{
			newObject.uniformBuffers.push_back(uBuffer);
			// Uniforms are persistently mapped, so we can just memcpy to them
			memcpy(uBuffer.transformBuffer.allocation->GetMappedData(), &transformation, transformBufferSize);
		}
		else
		{
			throw std::runtime_error("Failed to create descriptor set for test object");
		}
		
	}

	// Copy the data to the buffers
	copyBuffer(logDeviceM, newObject.vertexData, vertices.data());
	copyBuffer(logDeviceM, newObject.indexData, indices.data());
	// Now test object data should be available for the gpu.
	testObjectsM[id] = newObject;

	return true;
}

void GraphicsObjectManager::updateTestObject(int id, const void* srcData, uint32_t frame)
{
	auto& object = testObjectsM[id];
	auto& buffer = object.uniformBuffers[frame].transformBuffer;
	memcpy(buffer.allocation->GetMappedData(), srcData, buffer.size);
}

void GraphicsObjectManager::updateObjectDescriptor(int id, const void* srcData, uint32_t frame)
{
	auto& object = gameObjectsM[id]; // TODO update also the images or rework them.
	auto& buffer = object.uniformBuffers[frame].transformBuffer;
	memcpy(buffer.allocation->GetMappedData(), srcData, buffer.size);
}

void GraphicsObjectManager::copyBuffers(VkDevice logDevice, std::vector<AllocatedBuffer> buffers, std::vector<const void*> sources)
{
	std::vector<VkFence> fences;
	std::vector<AllocatedBuffer> allocatedStagingBuffers;
	
	int i = 0;
	for (auto allocObject : buffers)
	{
		VkMemoryPropertyFlags memProps;
		vmaGetAllocationMemoryProperties(allocatorM, allocObject.allocation, &memProps);
		VkDeviceSize bufferSize = allocObject.allocation->GetSize();

		if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) // mappable memory
		{
			void* pDataMap;
			vmaMapMemory(allocatorM, allocObject.allocation, &pDataMap);
			memcpy(pDataMap, sources[i], bufferSize);
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
			AllocatedBuffer allocatedStagingBuffer{};
			allocatedStagingBuffer.buffer = stagingBuffer;
			allocatedStagingBuffer.allocation = stagingBufferAllocation;
			allocatedStagingBuffer.size = bufferSize;
			allocatedStagingBuffers.push_back(allocatedStagingBuffer);
			// Copy the data to the staging buffer
			memcpy(allocInfo.pMappedData, sources[i], bufferSize);
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
		i++;
	} // end of for

	if (!fences.empty()) // If we added fences, wait for them to finish
	{ 
		vkWaitForFences(logDevice, fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
		for (auto fence : fences)
		{
			vkDestroyFence(logDevice, fence, nullptr);
		}
		for(auto buf : allocatedStagingBuffers)
		{
			vmaDestroyBuffer(allocatorM, buf.buffer, buf.allocation);
		}
	}
}

void GraphicsObjectManager::copyBuffer(VkDevice logDevice, AllocatedBuffer allocBuffer, const void* srcData)
{
	VkMemoryPropertyFlags memProps;
	vmaGetAllocationMemoryProperties(allocatorM, allocBuffer.allocation, &memProps);
	VkDeviceSize bufferSize = allocBuffer.size;

	if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) // mappable memory
	{
		void* pDataMap;
		vmaMapMemory(allocatorM, allocBuffer.allocation, &pDataMap);
		memcpy(pDataMap, srcData, bufferSize);
		// If the memory is not host coherent, we need to flush it
		if (!(memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			vmaFlushAllocation(allocatorM, allocBuffer.allocation, 0, bufferSize);

		vmaUnmapMemory(allocatorM, allocBuffer.allocation);
	}
	else // non-mappable memory, needs to be staged and copied using cmd buffers
	{
		AllocatedBuffer stagingBuffer{};
		VmaAllocationInfo allocInfo{};
		stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, allocInfo, VMA_MEMORY_USAGE_AUTO,
						VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
					| VMA_ALLOCATION_CREATE_MAPPED_BIT); // Also automatically map
		
		if (allocInfo.pMappedData == nullptr)
		{
			throw std::runtime_error("Failed to map staging buffer");
		}
		// Copy the data to the staging buffer
		memcpy(allocInfo.pMappedData, srcData, bufferSize); // intellisense complains about pMappedData being nullptr, but it can not be.
		vmaFlushAllocation(allocatorM, stagingBuffer.allocation, 0, bufferSize);
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

		vkCmdCopyBuffer(cmdBuffer, stagingBuffer.buffer, allocBuffer.buffer, 1, &copyRegion);

		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(transferQueueM, 1, &submitInfo, nullptr);
		// Todo add a way to define and wait for fence before destorying the staging buffer
		vkQueueWaitIdle(transferQueueM);
		// cleanup
		vmaDestroyBuffer(allocatorM, stagingBuffer.buffer, stagingBuffer.allocation);
		vkFreeCommandBuffers(logDeviceM, transferCommandPoolM, 1, &cmdBuffer);
	}
}

VkDescriptorSetLayout GraphicsObjectManager::getLayout(ObjectType type) const
{
	switch (type)
	{
		case ObjectType::TESTOBJECT:
			return testObjectSetLayoutM;
		case ObjectType::CHARACTER:
			return characterSetLayoutM;
		case ObjectType::GAMEOBJECT:
			return gameObjectSetLayoutM;
		default:
			return VK_NULL_HANDLE;
	}
}

std::vector<DrawableObject> GraphicsObjectManager::getDrawableObjects(ObjectType type, uint32_t frame) const
{
	std::vector<DrawableObject> drawables;

	switch (type)
	{
		case ObjectType::CHARACTER:
		{
			for (auto& characterPair : characterObjectsM)
			{
				auto character = characterPair.second;
				DrawableObject drawable{};
				drawable.vertexBuffer = character.vertexData.buffer;
				drawable.indexBuffer = character.indexData.buffer;
				drawable.indexCount = character.indexCount;
				drawable.descriptorSet = character.characterUniformBuffers[frame].descriptorSet;
				drawables.push_back(drawable);
			}
			break;
		}
		case ObjectType::GAMEOBJECT:
		{
			for (auto& objectPair : gameObjectsM)
			{
				auto object = objectPair.second;
				DrawableObject drawable{};
				drawable.vertexBuffer = object.vertexData.buffer;
				drawable.indexBuffer = object.indexData.buffer;
				drawable.indexCount = object.indexCount;
				drawable.descriptorSet = object.uniformBuffers[frame].descriptorSet;
				drawables.push_back(drawable);
			}
			break;
		}
		case ObjectType::TESTOBJECT:
		{
			for (auto& objectPair : testObjectsM)
			{
				auto object = objectPair.second;
				DrawableObject drawable{};
				drawable.vertexBuffer = object.vertexData.buffer;
				drawable.indexBuffer = object.indexData.buffer;
				drawable.indexCount = object.indexCount;
				drawable.descriptorSet = object.uniformBuffers[frame].descriptorSet;
				drawables.push_back(drawable);
			}
			break;
		}
		default:
			break;
	}

	return drawables;
}

#pragma endregion

inline std::array<VkDescriptorSetLayoutBinding, 2> CharacterObject::getDescriptorSetLayoutBindings()
{
	std::array<VkDescriptorSetLayoutBinding, 2> array;
	array[0].binding = 0;
	array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	array[0].descriptorCount = 1;
	array[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	array[0].pImmutableSamplers = nullptr;

	array[1].binding = 1;
	array[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	array[1].descriptorCount = 1;
	array[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	array[1].pImmutableSamplers = nullptr;

	return array;
}

inline std::array<VkDescriptorSetLayoutBinding, 2> GameObject::getDescriptorSetLayoutBindings()
{
	std::array<VkDescriptorSetLayoutBinding, 2> array;
	array[0].binding = 0;
	array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	array[0].descriptorCount = 1;
	array[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	array[0].pImmutableSamplers = nullptr;

	array[1].binding = 1;
	array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	array[1].descriptorCount = 1;
	array[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	array[1].pImmutableSamplers = nullptr;

	return array;
}
