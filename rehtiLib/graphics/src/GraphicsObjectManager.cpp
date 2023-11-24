#define VMA_IMPLEMENTATION
#include "GraphicsObjectManager.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#pragma region Manager

GraphicsObjectManager::GraphicsObjectManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue graphicsQueue, uint32_t graphicsQueueFamily, const uint32_t frameCount)
    : graphicsCommandUnitM({graphicsQueue, VK_NULL_HANDLE, graphicsQueueFamily}), logDeviceM(logDevice), frameCountM(frameCount)
{
    // Create a new command pool for the graphics queue
    VkCommandPoolCreateInfo cmdPoolCreateInfo{};
    cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamily;
    // Create the command pool and store it to the member variable
    if (vkCreateCommandPool(logDevice, &cmdPoolCreateInfo, nullptr, &graphicsCommandUnitM.commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool for graphics queue");
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

    // Create layouts for all object types
    createDescriptorBuilder();
    auto characterBindings = CharacterObject::getDescriptorSetLayoutBindings();
    auto gameObjectBindings = GameObject::getDescriptorSetLayoutBindings();
    auto testBindings = TestObject::getDescriptorSetLayoutBindings();
    auto areaBindings = AreaObject::getDescriptorSetLayoutBindings();
    pBuilderM->setDescriptorSetLayout(characterBindings.data(), characterBindings.size(), descriptorSetLayoutsM[ObjectType::CHARACTER]);
    pBuilderM->setDescriptorSetLayout(gameObjectBindings.data(), gameObjectBindings.size(), descriptorSetLayoutsM[ObjectType::GAMEOBJECT]);
    pBuilderM->setDescriptorSetLayout(testBindings.data(), testBindings.size(), descriptorSetLayoutsM[ObjectType::TESTOBJECT]);
    pBuilderM->setDescriptorSetLayout(areaBindings.data(), areaBindings.size(), descriptorSetLayoutsM[ObjectType::AREA]);
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
        vkDestroyImageView(logDeviceM, character.textureView, nullptr);
        for (auto& bufObject : character.characterUniformBuffers)
        {
            vmaDestroyBuffer(allocatorM, bufObject.boneTransformations.buffer, bufObject.boneTransformations.allocation);
            vmaDestroyBuffer(allocatorM, bufObject.transformBuffer.buffer, bufObject.transformBuffer.allocation);
        }
    }
    // Loop over objects
    for (auto& objectPair : gameObjectsM)
    {
        auto object = objectPair.second;
        vmaDestroyBuffer(allocatorM, object.vertexData.buffer, object.vertexData.allocation);
        vmaDestroyBuffer(allocatorM, object.indexData.buffer, object.indexData.allocation);
        vmaDestroyImage(allocatorM, object.texture.image, object.texture.allocation);
        vkDestroyImageView(logDeviceM, object.textureView, nullptr);
        for (auto& bufObject : object.uniformBuffers)
        {
            vmaDestroyBuffer(allocatorM, bufObject.transformBuffer.buffer, bufObject.transformBuffer.allocation);
        }
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
    // area objects
    for (auto& area : areaObjectsM)
    {
        vmaDestroyBuffer(allocatorM, area.vertexData.buffer, area.vertexData.allocation);
        vmaDestroyBuffer(allocatorM, area.indexData.buffer, area.indexData.allocation);
        for (size_t i = 0; i < area.textures.size(); i++)
        {
            vmaDestroyImage(allocatorM, area.textures[i].image, area.textures[i].allocation);
            vkDestroyImageView(logDeviceM, area.textureViews[i], nullptr);
        }
    }
    // Destroy command pools
    vkDestroyCommandPool(logDeviceM, graphicsCommandUnitM.commandPool, nullptr);
    if (transferCommandUnitM.has_value())
        vkDestroyCommandPool(logDeviceM, transferCommandUnitM.value().commandPool, nullptr);
    // And destroy the allocator
    vmaDestroyAllocator(allocatorM);
}

void GraphicsObjectManager::addTransferQueueFamilyAccess(const uint32_t transferQueueFamily, VkQueue transferQueue)
{
    // Check that it is not already set
    if (!transferCommandUnitM.has_value())
    {
        CommandUnit unit{};
        unit.queueFamilyIndex = transferQueueFamily;
        unit.queue = transferQueue;
        // Create a new command pool for the graphics queue
        VkCommandPoolCreateInfo cmdPoolCreateInfo{};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolCreateInfo.queueFamilyIndex = transferQueueFamily;
        // Create the command pool and store it to the member variable
        if (vkCreateCommandPool(logDeviceM, &cmdPoolCreateInfo, nullptr, &unit.commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool for transfer queue");
        }
        transferCommandUnitM = unit;
    }
}

bool GraphicsObjectManager::addCharacter(int characterID, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ImageData& texture, glm::mat4 transformation, glm::mat4 bindPose[MAX_BONES], VkSampler imgSampler)
{
    if (characterObjectsM.contains(characterID))
    {
        return false;
    }
    CharacterObject character{};
    VkDeviceSize vSize = vertices.size() * sizeof(Vertex);
    VkDeviceSize iSize = indices.size() * sizeof(uint32_t);
    VkDeviceSize tSize = sizeof(glm::mat4);
    VkDeviceSize imgSize = texture.width * texture.height * 4;
    VkDeviceSize boneSize = sizeof(glm::mat4) * MAX_BONES;
    character.indexCount = indices.size();

    // Create buffers
    character.vertexData = createBuffer(vSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    character.indexData = createBuffer(iSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    // image data
    character.texture = createImage(texture.width, texture.height, VK_FORMAT_R8G8B8A8_SRGB);
    character.textureView = createImageView(character.texture.image, VK_FORMAT_R8G8B8A8_SRGB);
    // Create descriptor set data
    for (uint32_t i = 0; i < frameCountM; i++)
    {
        // Go in binding order: transform, bones, texture
        CharacterObjectUniformBuffer cBuffer{};
        cBuffer.transformBuffer = createBuffer(tSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
                                               VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT); // These two flags set memory to be persistently mapped.
        cBuffer.boneTransformations = createBuffer(boneSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
                                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
        VkDescriptorBufferInfo transformBufferInfo{};
        transformBufferInfo.buffer = cBuffer.transformBuffer.buffer;
        transformBufferInfo.offset = 0;
        transformBufferInfo.range = tSize;
        VkDescriptorBufferInfo boneBufferInfo{};
        boneBufferInfo.buffer = cBuffer.boneTransformations.buffer;
        boneBufferInfo.offset = 0;
        boneBufferInfo.range = boneSize;
        VkDescriptorImageInfo descImageInfo{};
        descImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descImageInfo.imageView = character.textureView;
        descImageInfo.sampler = imgSampler;
        // Try creating descriptor set
        if (pBuilderM->bindBuffer(transformBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT) // Transform
                .bindBuffer(boneBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)            // Bones
                .bindImage(descImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)    // Texture
                .build(cBuffer.descriptorSet))
        {
            character.characterUniformBuffers.push_back(cBuffer);
            // Uniforms are persistently mapped, so we can just memcpy to them
            memcpy(cBuffer.transformBuffer.allocation->GetMappedData(), &transformation, tSize);
            memcpy(cBuffer.boneTransformations.allocation->GetMappedData(), bindPose, boneSize);
        }
        else
        {
            throw std::runtime_error("Failed to create descriptor set for a character object");
        }
    }

    // Copy the data to the buffers
    copyBuffer(character.vertexData, vertices.data());
    copyBuffer(character.indexData, indices.data());
    copyImage(character.texture, texture);

    characterObjectsM[characterID] = character;

    return true;
}

bool GraphicsObjectManager::addGameObject(int id, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ImageData& texture, glm::mat4 transformation, VkSampler imgSampler)
{
    if (gameObjectsM.contains(id))
    {
        return false;
    }
    GameObject gameObject{};
    VkDeviceSize vSize = vertices.size() * sizeof(Vertex);
    VkDeviceSize iSize = indices.size() * sizeof(uint32_t);
    VkDeviceSize tSize = sizeof(glm::mat4);
    VkDeviceSize imgSize = texture.width * texture.height * 4;
    gameObject.indexCount = indices.size();
    // Create buffers
    gameObject.vertexData = createBuffer(vSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    gameObject.indexData = createBuffer(iSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    // image data
    gameObject.texture = createImage(texture.width, texture.height, VK_FORMAT_R8G8B8A8_SRGB);
    gameObject.textureView = createImageView(gameObject.texture.image, VK_FORMAT_R8G8B8A8_SRGB);
    // Create descriptor set data
    for (uint32_t i = 0; i < frameCountM; i++)
    {
        GameObjectUniformBuffer uBuffer{};
        // We want uniform memory to be host coherent, and persistently mapped
        uBuffer.transformBuffer = createBuffer(tSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO,
                                               VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT); // These two flags set memory to be persistently mapped.
        VkDescriptorBufferInfo descBufferInfo{};
        descBufferInfo.buffer = uBuffer.transformBuffer.buffer;
        descBufferInfo.offset = 0;
        descBufferInfo.range = tSize;
        // Texture info
        VkDescriptorImageInfo descImageInfo{};
        descImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descImageInfo.imageView = gameObject.textureView;
        descImageInfo.sampler = imgSampler;
        // Try creating descriptor set
        if (pBuilderM->bindBuffer(descBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)   // Buffer
                .bindImage(descImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // Texture
                .build(uBuffer.descriptorSet))
        {
            gameObject.uniformBuffers.push_back(uBuffer);
            // Uniforms are persistently mapped, so we can just memcpy to them
            memcpy(uBuffer.transformBuffer.allocation->GetMappedData(), &transformation, tSize);
        }
        else
        {
            throw std::runtime_error("Failed to create descriptor set for a game object");
        }
    }

    // Copy the data to the buffers
    copyBuffer(gameObject.vertexData, vertices.data());
    copyBuffer(gameObject.indexData, indices.data());
    copyImage(gameObject.texture, texture);
    // Now the game object data should be available for the gpu.
    gameObjectsM[id] = gameObject;

    return true;
}

AllocatedBuffer GraphicsObjectManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationInfo& info, VmaMemoryUsage memUsage, VmaAllocationCreateFlags vmaCreationFlags, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
    AllocatedBuffer newBuffer{};

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = getSharingMode(); // returns concurrent if multiple queues are available
    // TODO Move all sharingmode stuff into a single functiom with configuration options
    if (transferCommandUnitM.has_value())
    {
        uint32_t queueFamilies[] = {graphicsCommandUnitM.queueFamilyIndex, transferCommandUnitM.value().queueFamilyIndex};
        bufferInfo.pQueueFamilyIndices = queueFamilies;
    }
    else
    {
        bufferInfo.pQueueFamilyIndices = &graphicsCommandUnitM.queueFamilyIndex;
    }
    bufferInfo.queueFamilyIndexCount = getQueueFamilyCount();
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
    bufferInfo.sharingMode = getSharingMode();
    // TODO Move all sharingmode stuff into a single functiom with configuration options
    if (transferCommandUnitM.has_value())
    {
        uint32_t queueFamilies[] = {graphicsCommandUnitM.queueFamilyIndex, transferCommandUnitM.value().queueFamilyIndex};
        bufferInfo.pQueueFamilyIndices = queueFamilies;
    }
    else
    {
        bufferInfo.pQueueFamilyIndices = &graphicsCommandUnitM.queueFamilyIndex;
    }
    bufferInfo.queueFamilyIndexCount = getQueueFamilyCount();
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

AllocatedImage GraphicsObjectManager::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage, VmaAllocationCreateFlags vmaCreationFlags, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
    AllocatedImage newImage{};
    // Always assume rgba texture. We can force an alpha channel without one existing
    VkDeviceSize size = width * height * 4;
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = 0;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1; // 2D image
    imageInfo.mipLevels = 1;    // No mip mapping
    imageInfo.arrayLayers = 1;  // No array layers
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We will be using pipeline barriers to transition the layout
    imageInfo.usage = usage;

    // TODO FIX THE SHARING MODES
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Concurrent does not allow for transfer queue to be used
    // Images are only to be sent in the graphics queue
    imageInfo.pQueueFamilyIndices = &graphicsCommandUnitM.queueFamilyIndex;
    imageInfo.queueFamilyIndexCount = 1;

    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // No multisampling

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.flags = vmaCreationFlags;
    allocInfo.usage = memUsage;
    allocInfo.requiredFlags = requiredFlags;
    allocInfo.preferredFlags = preferredFlags;

    VkResult vkResult = vmaCreateImage(allocatorM, &imageInfo, &allocInfo, &newImage.image, &newImage.allocation, nullptr);
    if (vkResult != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a VMA image: " + std::to_string(vkResult));
    }
    newImage.size = size;
    return newImage;
}

void GraphicsObjectManager::createDescriptorBuilder()
{
    PoolManager manager(logDeviceM);
    DescriptorSetLayoutCache cache(logDeviceM);
    pBuilderM = std::make_unique<DescriptorBuilder>(manager, cache);
}

uint32_t GraphicsObjectManager::getQueueFamilyCount()
{
    uint32_t count = 1;
    if (transferCommandUnitM.has_value())
    {
        count = 2;
    }
    return count;
}

VkSharingMode GraphicsObjectManager::getSharingMode() // TODO ADD FUNCTIONALITY TO DEFINE FAMILY PREFERENCE
{
    VkSharingMode mode;
    if (transferCommandUnitM.has_value()) // If we have separate transfer queue
    {
        mode = VK_SHARING_MODE_CONCURRENT;
    }
    else // if we only have graphics queue
    {
        mode = VK_SHARING_MODE_EXCLUSIVE;
    }
    return mode;
}

VkCommandBuffer GraphicsObjectManager::startCommandBuffer(bool preferTransfer)
{
    VkCommandBuffer cmdBuffer;
    if (preferTransfer && transferCommandUnitM.has_value())
    {
        cmdBuffer = transferCommandUnitM.value().startCommandBuffer(logDeviceM);
        activeCommandUnitM = transferCommandUnitM.value();
    }
    else
    {
        cmdBuffer = graphicsCommandUnitM.startCommandBuffer(logDeviceM);
        activeCommandUnitM = graphicsCommandUnitM;
    }

    return cmdBuffer;
}

bool GraphicsObjectManager::endCommandBuffer(VkCommandBuffer commandBuffer, VkFence fence)
{
    bool val = false;
    if (activeCommandUnitM.has_value()) // We can only end if we have an active unit.
    {
        val = activeCommandUnitM.value().endCommandBuffer(logDeviceM, commandBuffer, fence);
        activeCommandUnitM.reset();
    }
    return val;
}

void GraphicsObjectManager::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer, VkImageAspectFlags aspectFlags, std::pair<uint32_t, uint32_t> srcAndDstQueueFamilies)
{

    std::pair<VkAccessFlags, VkAccessFlags> srcAndDstAccessMasks{};
    std::pair<VkPipelineStageFlags, VkPipelineStageFlags> srcAndDstStages{};
    bool res = getPipelineAndAccessFlags(oldLayout, newLayout, srcAndDstAccessMasks, srcAndDstStages);
    if (!res)
    {
        throw std::runtime_error("Failed to get pipeline and access flags for image layout transition");
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = srcAndDstQueueFamilies.first;
    barrier.dstQueueFamilyIndex = srcAndDstQueueFamilies.second;
    // data
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = 0; // No mip mapping
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0; // No array layers
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = srcAndDstAccessMasks.first;
    barrier.dstAccessMask = srcAndDstAccessMasks.second;

    vkCmdPipelineBarrier(commandBuffer,
                         srcAndDstStages.first, srcAndDstStages.second,
                         0, 0, nullptr, 0, nullptr,
                         1, &barrier);
}

std::pair<uint32_t, uint32_t> GraphicsObjectManager::getQueueTransitionFamilies()
{
    if (transferCommandUnitM.has_value())
    { // Match previous transition
        return std::make_pair(transferCommandUnitM.value().queueFamilyIndex, graphicsCommandUnitM.queueFamilyIndex);
    }
    return std::make_pair(VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED);
}

bool GraphicsObjectManager::getPipelineAndAccessFlags(VkImageLayout srcLayout, VkImageLayout dstLayout, std::pair<VkAccessFlags, VkAccessFlags>& srcAndDstAccessMasks, std::pair<VkPipelineStageFlags, VkPipelineStageFlags>& srcAndDstStages)
{
    // transfer image
    if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        dstLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        srcAndDstAccessMasks.first = 0;
        srcAndDstAccessMasks.second = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcAndDstStages.first = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        srcAndDstStages.second = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (srcLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAndDstAccessMasks.first = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcAndDstAccessMasks.second = VK_ACCESS_SHADER_READ_BIT;

        srcAndDstStages.first = VK_PIPELINE_STAGE_TRANSFER_BIT;
        srcAndDstStages.second = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             dstLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAndDstAccessMasks.first = 0;
        srcAndDstAccessMasks.second = VK_ACCESS_SHADER_READ_BIT;

        srcAndDstStages.first = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        srcAndDstStages.second = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (srcLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             dstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        srcAndDstAccessMasks.first = 0;
        srcAndDstAccessMasks.second = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        srcAndDstStages.first = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        srcAndDstStages.second = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        return false;
    }

    return true;
}

AllocatedBuffer GraphicsObjectManager::createStagingBuffer(VkDeviceSize size, VmaAllocationInfo& allocInfo)
{
    AllocatedBuffer stagingBuffer{};
    stagingBuffer = createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, allocInfo, VMA_MEMORY_USAGE_AUTO,
                                 VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT); // Also automatically map

    if (allocInfo.pMappedData == nullptr)
    {
        throw std::runtime_error("Failed to map staging buffer");
    }
    return stagingBuffer;
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
    copyBuffer(newObject.vertexData, vertices.data());
    copyBuffer(newObject.indexData, indices.data());
    // Now test object data should be available for the gpu.
    testObjectsM[id] = newObject;

    return true;
}

bool GraphicsObjectManager::addArea(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::array<ImageData, 6> textures, VkSampler texSampler)
{
    VkDeviceSize vSize = vertices.size() * sizeof(Vertex);
    VkDeviceSize iSize = indices.size() * sizeof(uint32_t);
    AreaObject area{};
    area.indexCount = indices.size();
    area.vertexData = createBuffer(vSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    area.indexData = createBuffer(iSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    std::array<VkDescriptorImageInfo, 6> imageInfos{};
    for (size_t i = 0u; i < textures.size(); i++)
    {
        area.textures[i] = createImage(textures[i].width, textures[i].height, VK_FORMAT_R8G8B8A8_SRGB);
        area.textureViews[i] = createImageView(area.textures[i].image, VK_FORMAT_R8G8B8A8_SRGB);
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView = area.textureViews[i];
        imageInfos[i].sampler = texSampler;
    }

    pBuilderM->bindImages(imageInfos.data(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, imageInfos.size())
        .build(area.descriptorSet);
    // Copy the data to the buffers
    copyBuffer(area.vertexData, vertices.data());
    copyBuffer(area.indexData, indices.data());
    for (size_t i = 0u; i < textures.size(); i++)
    {
        copyImage(area.textures[i], textures[i]);
    }
    areaObjectsM.push_back(area);

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
    GameObject& object = gameObjectsM[id]; // TODO update also the images or rework them.
    auto& buffer = object.uniformBuffers[frame].transformBuffer;
    memcpy(buffer.allocation->GetMappedData(), srcData, buffer.size);
}

void GraphicsObjectManager::updateCharacterDescriptor(int id, const void* transformSrcData, const void* boneSrcData, uint32_t frame)
{
    CharacterObject& character = characterObjectsM[id];
    auto& transformBuffer = character.characterUniformBuffers[frame].transformBuffer;
    auto& boneBuffer = character.characterUniformBuffers[frame].boneTransformations;
    memcpy(transformBuffer.allocation->GetMappedData(), transformSrcData, transformBuffer.size);
    memcpy(boneBuffer.allocation->GetMappedData(), boneSrcData, boneBuffer.size);
}

AllocatedImage GraphicsObjectManager::createDepthImage(uint32_t width, uint32_t height, VkFormat depthFormat)
{
    AllocatedImage newImage{};
    newImage = createImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    return newImage;
}

void GraphicsObjectManager::destroyImage(AllocatedImage image)
{
    vmaDestroyImage(allocatorM, image.image, image.allocation);
}

void GraphicsObjectManager::transitionDepthImageLayout(AllocatedImage depthImage, VkFormat depthFormat, VkImageLayout srcLayout, VkImageLayout dstLayout)
{
    VkCommandBuffer cmdBuffer = startCommandBuffer(false);
    transitionImageLayout(depthImage.image, depthFormat, srcLayout, dstLayout, cmdBuffer, VK_IMAGE_ASPECT_DEPTH_BIT);
    endCommandBuffer(cmdBuffer);
}

VkImageView GraphicsObjectManager::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format;
    info.subresourceRange.aspectMask = aspectFlags;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    VkImageView view;
    if (vkCreateImageView(logDeviceM, &info, nullptr, &view))
    {
        throw std::runtime_error("Failed to create an image view");
    }
    return view;
}

void GraphicsObjectManager::copyBuffer(AllocatedBuffer allocBuffer, const void* srcData)
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
        VmaAllocationInfo allocInfo;
        AllocatedBuffer stagingBuffer = createStagingBuffer(bufferSize, allocInfo);
        // Copy the data to the staging buffer
        memcpy(allocInfo.pMappedData, srcData, bufferSize); // intellisense complains about pMappedData being nullptr, but it can not be.
        vmaFlushAllocation(allocatorM, stagingBuffer.allocation, 0, bufferSize);
        // Perhaps the following section should be separated into its own function

        // Start recording
        VkCommandBuffer cmdBuffer = startCommandBuffer(true); // prefer transfer

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;

        vkCmdCopyBuffer(cmdBuffer, stagingBuffer.buffer, allocBuffer.buffer, 1, &copyRegion);
        // End, submit and destroy
        endCommandBuffer(cmdBuffer);
        // cleanup staging buffer
        vmaDestroyBuffer(allocatorM, stagingBuffer.buffer, stagingBuffer.allocation);
    }
}

void GraphicsObjectManager::copyImage(AllocatedImage allocImage, const ImageData& srcData)
{
    uint32_t width, height;
    width = srcData.width;
    height = srcData.height;

    VkMemoryPropertyFlags memProps;
    vmaGetAllocationMemoryProperties(allocatorM, allocImage.allocation, &memProps);
    VkDeviceSize imageSize = allocImage.size;

    if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // The real question is how to get the image to be correctly formatted if this is the case.
        void* data;
        vmaMapMemory(allocatorM, allocImage.allocation, &data);
        memcpy(data, srcData.pixels, imageSize);
        if (!(memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            vmaFlushAllocation(allocatorM, allocImage.allocation, 0, imageSize);
        vmaUnmapMemory(allocatorM, allocImage.allocation);
        // Make a transition layout
        VkCommandBuffer cmdBuffer = startCommandBuffer(false);
        // TODO If sharing mode is exclusive, and we have multiple queues, we need to transfer ownership.
        transitionImageLayout(allocImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmdBuffer);
        endCommandBuffer(cmdBuffer);
    }
    else
    {
        VmaAllocationInfo allocInfo{};
        AllocatedBuffer stagingBuffer = createStagingBuffer(imageSize, allocInfo);
        // Copy the data to the staging buffer.
        memcpy(allocInfo.pMappedData, srcData.pixels, imageSize);
        vmaFlushAllocation(allocatorM, stagingBuffer.allocation, 0, imageSize);
        // Start command buffer
        VkCommandBuffer cmdBuffer = startCommandBuffer(false);
        // Transfer image layout from undefined to transfer dst optimal. (and transfer ownership if needed)
        transitionImageLayout(allocImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmdBuffer);

        VkBufferImageCopy imgCopy{};
        imgCopy.bufferOffset = 0;
        imgCopy.bufferRowLength = 0;
        imgCopy.bufferImageHeight = 0;
        imgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgCopy.imageSubresource.mipLevel = 0;
        imgCopy.imageSubresource.baseArrayLayer = 0;
        imgCopy.imageSubresource.layerCount = 1;

        imgCopy.imageOffset = {0, 0, 0};
        imgCopy.imageExtent = {
            width,
            height,
            1};
        vkCmdCopyBufferToImage(
            cmdBuffer,
            stagingBuffer.buffer,
            allocImage.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imgCopy);
        endCommandBuffer(cmdBuffer);
        // Make it available for the shader
        VkCommandBuffer cmdBuffer2 = startCommandBuffer(false);
        transitionImageLayout(allocImage.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmdBuffer);
        // End recording
        endCommandBuffer(cmdBuffer2);
        // clean up staging buffer
        vmaDestroyBuffer(allocatorM, stagingBuffer.buffer, stagingBuffer.allocation);
    }
}

VkDescriptorSetLayout GraphicsObjectManager::getLayout(ObjectType type) const
{
    if (type != ObjectType::UNDEFINED)
    {
        return descriptorSetLayoutsM[type];
    }
    else
    {
        return VK_NULL_HANDLE;
    }
}

uint32_t GraphicsObjectManager::getLayoutCount(ObjectType type) const
{
    if (type != ObjectType::UNDEFINED)
    {
        return 1;
    }
    else
    {
        return 0;
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
    case ObjectType::AREA:
    {
        for (auto& areaObj : areaObjectsM)
        {
            DrawableObject drawable{};
            drawable.vertexBuffer = areaObj.vertexData.buffer;
            drawable.indexBuffer = areaObj.indexData.buffer;
            drawable.indexCount = areaObj.indexCount;
            drawable.descriptorSet = areaObj.descriptorSet;
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

VkCommandBuffer GraphicsObjectManager::CommandUnit::startCommandBuffer(VkDevice logDevice)
{
    // Allocate a command buffer
    VkCommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;
    cmdAllocInfo.commandPool = commandPool;

    VkCommandBuffer cmdBuffer;
    if (vkAllocateCommandBuffers(logDevice, &cmdAllocInfo, &cmdBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate a command buffer.");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (VK_SUCCESS != vkBeginCommandBuffer(cmdBuffer, &beginInfo))
    {
        throw std::runtime_error("Failed to begin command buffer");
    }
    return cmdBuffer;
}

bool GraphicsObjectManager::CommandUnit::endCommandBuffer(VkDevice logDevice, VkCommandBuffer commandBuffer, VkFence fence)
{
    if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer))
    {
        return false;
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, fence);

    if (fence == VK_NULL_HANDLE) // No fence, idle
    {
        vkQueueWaitIdle(queue);
    }

    vkFreeCommandBuffers(logDevice, commandPool, 1, &commandBuffer);
    return true;
}
