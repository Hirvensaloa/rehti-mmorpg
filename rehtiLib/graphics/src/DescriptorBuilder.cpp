#include "DescriptorBuilder.hpp"

#include <algorithm>
#include <functional>
#include <stdexcept>

PoolManager::PoolManager(VkDevice device)
    : logDeviceM(device), currentPoolM(VK_NULL_HANDLE)
{
}

PoolManager::~PoolManager()
{
    for (auto& pool : usedPoolsM)
    {
        vkDestroyDescriptorPool(logDeviceM, pool, nullptr);
    }

    for (auto& pool : freePoolsM)
    {
        vkDestroyDescriptorPool(logDeviceM, pool, nullptr);
    }
}

void PoolManager::resetPools()
{
    for (auto pool : usedPoolsM)
    {
        // Fun fact: this function signature has flags, but the spec states that they are for future use.
        vkResetDescriptorPool(logDeviceM, pool, 0);
        freePoolsM.push_back(pool);
    }
    usedPoolsM.clear();
    currentPoolM = VK_NULL_HANDLE;
}

bool PoolManager::allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& descSet)
{
    // Make sure we have a pool
    if (currentPoolM == VK_NULL_HANDLE)
    {
        currentPoolM = grabPool();
        usedPoolsM.push_back(currentPoolM);
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = currentPoolM;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    // Try alloc
    VkResult allocResult = vkAllocateDescriptorSets(logDeviceM, &allocInfo, &descSet);
    bool realloc = false;

    switch (allocResult)
    {
    case VK_SUCCESS:
        return true;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        realloc = true;
        break;
    default:
        return false;
    }

    if (realloc)
    {
        // Grab new pool and retry
        currentPoolM = grabPool();
        usedPoolsM.push_back(currentPoolM);
        allocInfo.descriptorPool = currentPoolM;
        allocResult = vkAllocateDescriptorSets(logDeviceM, &allocInfo, &descSet);
        if (allocResult == VK_SUCCESS)
        {
            return true;
        }
    }

    return false;
}

VkDescriptorPool PoolManager::createPool(VkDevice device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes)
{
    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(device, &createInfo, nullptr, &pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool");
    }
    freePoolsM.push_back(pool);

    return pool;
}

VkDescriptorPool PoolManager::grabPool()
{
    VkDescriptorPool pool;
    if (freePoolsM.empty())
    {
        pool = createPool(logDeviceM, 0, 1000, STANDARD_POOL_SIZES);
    }
    else
    {
        auto lastDescpool = freePoolsM.back();
        freePoolsM.pop_back();
        pool = lastDescpool;
    }
    return pool;
}

DescriptorSetLayoutCache::DescriptorSetLayoutCache(VkDevice device)
    : logDeviceM(device)
{
}

DescriptorSetLayoutCache::~DescriptorSetLayoutCache()
{
    for (auto& layout : layoutCacheM)
    {
        vkDestroyDescriptorSetLayout(logDeviceM, layout.second, nullptr);
    }
}

VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& layoutInfo)
{
    DescriptorSetLayoutInfo info{};
    info.bindings.reserve(layoutInfo.bindingCount);

    bool sorted = true;
    int lastBinding = -1;

    for (uint32_t i = 0; i < layoutInfo.bindingCount; i++)
    {
        VkDescriptorSetLayoutBinding binding = layoutInfo.pBindings[i];
        info.bindings.push_back(binding);

        if (lastBinding < binding.binding) // check order
        {
            sorted = false;
        }
        lastBinding = static_cast<int>(binding.binding);
    }

    if (!sorted)
        std::sort(info.bindings.begin(), info.bindings.end(), [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
                  { return a.binding < b.binding; });
    // Info is now sanitized and sorted
    VkDescriptorSetLayout layout;

    auto layoutIt = layoutCacheM.find(info);
    if (layoutIt != layoutCacheM.end())
    { // found
        return layoutIt->second;
    }
    else
    { // not found
        if (vkCreateDescriptorSetLayout(logDeviceM, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout");
        }
        layoutCacheM[info] = layout;
    }
    return layout;
}

VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindingCount);
    layoutInfo.pBindings = bindings;

    return createDescriptorSetLayout(layoutInfo);
}

bool DescriptorSetLayoutCache::DescriptorSetLayoutInfo::operator==(const DescriptorSetLayoutInfo& other) const
{
    if (bindings.size() != other.bindings.size())
    {
        return false;
    }
    for (uint32_t i = 0; i < bindings.size(); i++)
    {
        VkDescriptorSetLayoutBinding bindingThis = bindings[i];
        VkDescriptorSetLayoutBinding bindingThat = other.bindings[i];

        if (bindingThis.binding != bindingThat.binding ||
            bindingThis.descriptorCount != bindingThat.descriptorCount ||
            bindingThis.descriptorType != bindingThat.descriptorType ||
            bindingThis.stageFlags != bindingThat.stageFlags)
        {
            return false;
        }
    }

    return true;
}

size_t DescriptorSetLayoutCache::DescriptorSetLayoutInfo::hash() const
{
    size_t res = std::hash<size_t>()(bindings.size());

    for (const VkDescriptorSetLayoutBinding& bind : bindings)
    {
        size_t byteSize = sizeof(char);
        size_t bindHash = bind.binding | bind.descriptorCount << 8 | bind.descriptorType << 16 | bind.stageFlags << 24;
        res ^= std::hash<size_t>()(bindHash);
    }
    return res;
}

DescriptorBuilder::DescriptorBuilder(PoolManager& poolManager, DescriptorSetLayoutCache& cache)
    : currentBindingM(0)
{
    pPoolManagerM = std::make_unique<PoolManager>(poolManager);
    pLayoutCacheM = std::make_unique<DescriptorSetLayoutCache>(cache);
}

DescriptorBuilder::~DescriptorBuilder()
{
}

DescriptorBuilder& DescriptorBuilder::bindBuffer(VkDescriptorBufferInfo& bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding newLayoutBinding = {};

    newLayoutBinding.descriptorCount = 1;
    newLayoutBinding.descriptorType = type;
    newLayoutBinding.stageFlags = stageFlags;
    newLayoutBinding.pImmutableSamplers = nullptr;
    newLayoutBinding.binding = currentBindingM;

    layoutBindingsM.push_back(newLayoutBinding);

    VkWriteDescriptorSet newWriteSet = {};
    newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWriteSet.pNext = nullptr;
    newWriteSet.descriptorCount = 1;
    newWriteSet.descriptorType = type;
    newWriteSet.dstBinding = currentBindingM; // naturally, layout binding, and the write set binding should match.
    newWriteSet.pBufferInfo = &bufferInfo;

    writeSetsM.push_back(newWriteSet);
    currentBindingM++; // Set the next binding to be used
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bindBuffers(const VkDescriptorBufferInfo* bufferInfos, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count)
{
    VkDescriptorSetLayoutBinding newLayoutBinding = {};

    newLayoutBinding.descriptorCount = count;
    newLayoutBinding.descriptorType = type;
    newLayoutBinding.stageFlags = stageFlags;
    newLayoutBinding.pImmutableSamplers = nullptr;
    newLayoutBinding.binding = currentBindingM;

    layoutBindingsM.push_back(newLayoutBinding);

    VkWriteDescriptorSet newWriteSet = {};
    newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWriteSet.pNext = nullptr;
    newWriteSet.descriptorCount = count;
    newWriteSet.descriptorType = type;
    newWriteSet.dstBinding = currentBindingM; // naturally, layout binding, and the write set binding should match.
    newWriteSet.pBufferInfo = bufferInfos;

    writeSetsM.push_back(newWriteSet);
    currentBindingM++; // Set the next binding to be used
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImage(VkDescriptorImageInfo& imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding newLayoutBinding = {};

    newLayoutBinding.descriptorCount = 1;
    newLayoutBinding.descriptorType = type;
    newLayoutBinding.stageFlags = stageFlags;
    newLayoutBinding.pImmutableSamplers = nullptr;
    newLayoutBinding.binding = currentBindingM;

    layoutBindingsM.push_back(newLayoutBinding);

    VkWriteDescriptorSet newWriteSet = {};
    newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWriteSet.pNext = nullptr;
    newWriteSet.descriptorCount = 1;
    newWriteSet.descriptorType = type;
    newWriteSet.dstBinding = currentBindingM;
    newWriteSet.pImageInfo = &imageInfo;
    // Dst set is not set yet, but will be at build time

    writeSetsM.push_back(newWriteSet);
    currentBindingM++; // Set the next binding to be used
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImages(const VkDescriptorImageInfo* imageInfos, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count)
{
    VkDescriptorSetLayoutBinding newLayoutBinding = {};

    newLayoutBinding.descriptorCount = count;
    newLayoutBinding.descriptorType = type;
    newLayoutBinding.stageFlags = stageFlags;
    newLayoutBinding.pImmutableSamplers = nullptr;
    newLayoutBinding.binding = currentBindingM;

    layoutBindingsM.push_back(newLayoutBinding);

    VkWriteDescriptorSet newWriteSet = {};
    newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    newWriteSet.pNext = nullptr;
    newWriteSet.descriptorCount = count;
    newWriteSet.descriptorType = type;
    newWriteSet.dstBinding = currentBindingM;
    newWriteSet.pImageInfo = imageInfos;
    // Dst set is not set yet, but will be at build time

    writeSetsM.push_back(newWriteSet);
    currentBindingM++; // Set the next binding to be used
    return *this;
}

bool DescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindingsM.size());
    layoutInfo.pBindings = layoutBindingsM.data();

    layout = pLayoutCacheM->createDescriptorSetLayout(layoutInfo); // caches the layout if created
    // Allocate the set
    if (!pPoolManagerM->allocateDescriptorSet(layout, set))
    {
        return false;
    }

    // And then handle the writes
    for (VkWriteDescriptorSet& write : writeSetsM)
    {
        write.dstSet = set;
    }
    // updates the set
    vkUpdateDescriptorSets(pPoolManagerM->getDevice(), static_cast<uint32_t>(writeSetsM.size()), writeSetsM.data(), 0, nullptr);

    // Clear the vectors
    layoutBindingsM.clear();
    writeSetsM.clear();
    currentBindingM = 0;

    return true;
}

bool DescriptorBuilder::build(VkDescriptorSet& set)
{
    VkDescriptorSetLayout layout;
    return build(set, layout);
}

void DescriptorBuilder::setDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount, VkDescriptorSetLayout& layout)
{
    layout = pLayoutCacheM->createDescriptorSetLayout(bindings, bindingCount);
}

const PoolManager& DescriptorBuilder::getPoolManager() const
{
    return *pPoolManagerM;
}
