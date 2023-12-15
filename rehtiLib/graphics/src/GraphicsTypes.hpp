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
// Image handle and allocation (and size)
struct AllocatedImage
{
    VkImage image;
    VmaAllocation allocation;
    VkDeviceSize size;
};
// Has image view in addition to AllocatedImage struct
struct CombinedImage
{
    AllocatedImage imageAllocation;
    VkImageView imageView;
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

// Simple uniform buffer with a single allocated buffer
struct SimpleUniformBuffer
{
    VkDescriptorSet descriptorSet; // Descriptor set of the data
    AllocatedBuffer buffer;        // Buffer containing whatever data is needed
    void* mappedTransformData;     // Pointer to the mapped data of the transform buffer
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
    AllocatedBuffer materialData;
    uint32_t indexCount;
    AllocatedImage texture;
    VkImageView textureView;
    PhongMaterial material;
    std::vector<CharacterObjectUniformBuffer> characterUniformBuffers;
    static std::array<VkDescriptorSetLayoutBinding, 4> getDescriptorSetLayoutBindings();
};

struct GameObject
{
    AllocatedBuffer vertexData;
    AllocatedBuffer indexData;
    AllocatedBuffer materialData;
    uint32_t indexCount;
    AllocatedImage texture;
    VkImageView textureView;
    PhongMaterial material;
    std::vector<GameObjectUniformBuffer> uniformBuffers;
    static std::array<VkDescriptorSetLayoutBinding, 3> getDescriptorSetLayoutBindings();
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
    std::vector<SimpleUniformBuffer> uniformBuffers;
    static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

/**
 * @brief Simple light object that contains a list of uniform buffers.
 */
struct LightObject
{
    std::vector<SimpleUniformBuffer> uniformBuffers; // List of light data along with their descriptor sets
    static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

/**
 * @brief Returns a vector of VkVertexInputAttributeDescription for the given object type.
 * @param objectType to query the attribute descriptions for.
 */
std::vector<VkVertexInputAttributeDescription> getAttributeDescription(ObjectType objectType);

/**
 * @brief Returns the vertex binding description for the given object type.
 * @param objectType to query the binding description for.
 * @return
 */
VkVertexInputBindingDescription getBindingDescription(ObjectType objectType);