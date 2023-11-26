#pragma once
#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>

// Chunk size for descriptor pool allocations
constexpr VkDeviceSize POOL_CHUNK_SIZE = 256;

// Standard pool sizes for descriptor types
const std::vector<VkDescriptorPoolSize> STANDARD_POOL_SIZES({{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                             {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                             {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                             {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}});

/// <summary>
/// This class manages descriptor pools, and allows for the allocation of descriptor sets.
/// </summary>
class PoolManager
{
  public:
    PoolManager(VkDevice device);
    ~PoolManager();

    /// <summary>
    /// Resets pools
    /// </summary>
    void resetPools();

    /// <summary>
    /// Allocates a descriptor set from the pool
    /// </summary>
    /// <param name="layout">Layout to be used</param>
    /// <param name="descSet">Descriptor set to be allocated</param>
    /// <returns></returns>
    bool allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& descSet);

    /// <summary>
    /// Allocates a descriptor pool and adds it to the free pools.
    /// </summary>
    /// <param name="device">logical device</param>
    /// <param name="flags">flags</param>
    /// <param name="maxSets">maximum number of sets to be allocated from this pool</param>
    /// <param name="poolSizes">List of limits on individual descriptors that can be allocated from this pool. For
    /// example 2 uniform buffers or 3 images.</param> <returns></returns>
    VkDescriptorPool createPool(VkDevice device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets,
                                const std::vector<VkDescriptorPoolSize>& poolSizes);

    /// <summary>
    /// Returns the logical device used by this manager.
    /// </summary>
    /// <returns></returns>
    VkDevice getDevice() const
    {
        return logDeviceM;
    }

  private:
    /// <summary>
    /// Returns a pool from free pools or creates a new one.
    /// Does not push the returned pool to used pools.
    /// </summary>
    /// <returns></returns>
    VkDescriptorPool grabPool();

    VkDevice logDeviceM;
    VkDescriptorPool currentPoolM;
    std::vector<VkDescriptorPool> freePoolsM;
    std::vector<VkDescriptorPool> usedPoolsM;
};

/// <summary>
/// The use of this class is to cache descriptor set layouts.
/// </summary>
class DescriptorSetLayoutCache
{
  public:
    DescriptorSetLayoutCache(VkDevice device);
    ~DescriptorSetLayoutCache();

    /// <summary>
    /// Creates a descriptor set layout from the given info, or returns one from the cache if it already exists.
    /// </summary>
    /// <param name="layoutInfo">Layout info</param>
    /// <returns>Allocated descriptor set layout</returns>
    VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& layoutInfo);

    /// <summary>
    /// Creates a descriptor set layout from the given bindings, or returns one from the cache if it already exists.
    /// </summary>
    /// <param name="bindings"> to create a layout of.</param>
    /// <param name="bindingCount"> of the bindings.</param>
    /// <returns>Created layout.</returns>
    VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings,
                                                    uint32_t bindingCount);

    struct DescriptorSetLayoutInfo
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bool operator==(const DescriptorSetLayoutInfo& other) const;
        size_t hash() const;
    };

  private:
    struct DescriptorLayoutHasher
    {
        size_t operator()(const DescriptorSetLayoutInfo& info) const
        {
            return info.hash();
        }
    };

    VkDevice logDeviceM;
    std::unordered_map<DescriptorSetLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHasher> layoutCacheM;
};

/// <summary>
/// DescriptorBuilder uses PoolManager and DescriptorLayoutCache to manage and create descriptor sets.
/// </summary>
class DescriptorBuilder
{
  public:
    DescriptorBuilder(PoolManager& poolManager, DescriptorSetLayoutCache& cache);
    ~DescriptorBuilder();

    /// <summary>
    /// Creates a descriptor set layout binding and a write descriptor set for a buffer.
    /// </summary>
    /// <param name="bufferInfo">struct decribing the buffer data.</param>
    /// <param name="type">of descriptor</param>
    /// <param name="stageFlags">Descriptor stage</param>
    /// <returns>The builder itself.</returns>
    DescriptorBuilder& bindBuffer(VkDescriptorBufferInfo& bufferInfo, VkDescriptorType type,
                                  VkShaderStageFlags stageFlags);

    /// <summary>
    /// Creates a descriptor set layout binding and a write descriptor set for an image.
    /// </summary>
    /// <param name="imageInfo">struct decribing the image data.</param>
    /// <param name="type">of descriptor.</param>
    /// <param name="stageFlags">Descriptor stage.</param>
    /// <returns>The builder itself.</returns>
    DescriptorBuilder& bindImage(VkDescriptorImageInfo& imageInfo, VkDescriptorType type,
                                 VkShaderStageFlags stageFlags);

    /**
     * @brief Binds multiple images to the descriptor set as an array.
     * @param imageInfos is an array of image info structs.
     * @param type of resource.
     * @param stageFlags is the shader stage.
     * @param count is the number of images to bind. Must match what is in the imageInfos array.
     * @return the builder itself.
     */
    DescriptorBuilder& bindImages(const VkDescriptorImageInfo* imageInfos, VkDescriptorType type,
                                  VkShaderStageFlags stageFlags, uint32_t count);

    /// <summary>
    /// Builds a descriptor set and sets a created layout to the supplied parameter.
    /// Also updates the descriptor sets.
    /// </summary>
    /// <param name="set"> to be created.</param>
    /// <param name="layout"> to be filled.</param>
    /// <returns>Boolean indicating operation status.</returns>
    bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);

    /// <summary>
    /// Builds a descriptor set.
    /// </summary>
    /// <param name="set">set to be built</param>
    /// <returns>Boolean indicating operation status</returns>
    bool build(VkDescriptorSet& set);

    /// <summary>
    /// Sets a layout to the supplied parameter from the given bindings.
    /// </summary>
    /// <param name="layout"> to be set</param>
    /// <param name="bindings"> to be used</param>
    void setDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount,
                                VkDescriptorSetLayout& layout);

    const PoolManager& getPoolManager() const;

  private:
    uint32_t currentBindingM;
    std::vector<VkWriteDescriptorSet> writeSetsM;
    std::vector<VkDescriptorSetLayoutBinding> layoutBindingsM;

    std::unique_ptr<PoolManager> pPoolManagerM;
    std::unique_ptr<DescriptorSetLayoutCache> pLayoutCacheM;
};