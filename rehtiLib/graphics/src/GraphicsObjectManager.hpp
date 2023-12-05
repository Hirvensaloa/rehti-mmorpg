#pragma once
#include "DescriptorBuilder.hpp"
#include "GraphicsTypes.hpp"
#include "Mesh.hpp"

#include <array>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief Graphics object manager manages different types of objects that have memory and can be drawn.
 */
class GraphicsObjectManager
{
public:
    /**
     * @brief Creates a graphics object manager.
     * @param instance is a Vulkan instance.
     * @param gpu is the physical device.
     * @param logDevice is the logical device.
     * @param graphicsQueue is the queue for graphics operations.
     * @param graphicsMutex is a mutex for graphics operations.
     * @param graphicsQueueFamily is the family index of the graphics queue.
     * @param frameCount is the number of frames.
     */
    GraphicsObjectManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue graphicsQueue, std::shared_mutex& graphicsMutex, uint32_t graphicsQueueFamily, const uint32_t frameCount);

    /**
     * @brief Destructor.
     */
    ~GraphicsObjectManager();

    /**
     * @brief Adds the given transfer queue to the manager.
     * @param transferQueueFamily is the family index of the transfer queue.
     * @param transferQueue is the transfer queue.
     */
    void addTransferQueueFamilyAccess(const uint32_t transferQueueFamily, VkQueue transferQueue);

    /**
     * @brief Adds a character to the buffer manager with the given parameters.
     * @param characterID is the id of the character
     * @param vertices of the character
     * @param indices of the character
     * @param texture of the character
     * @param transformation to be applied to the character
     * @param bindPose is the bind pose bone data of the character
     * @param imgSampler is the sampler for the texture of the character
     * @return boolean indicating success of the operation
     */
    bool addCharacter(int characterID, const std::vector<CharacterVertex>& vertices, const std::vector<uint32_t>& indices, ImageData& texture, glm::mat4 transformation, glm::mat4 bindPose[MAX_BONES], VkSampler imgSampler);

    /**
     * @brief Adds a game object to the buffer manager
     * @param objectId
     * @param vertices of the object
     * @param indices of the points making up triangles
     * @param texture of the object
     * @param transformation to be applied to the object
     * @param imgSampler is the sampler for the texture of the gameobject
     * @return boolean indicating success of the operation
     */
    bool addGameObject(int objectId, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ImageData& texture, glm::mat4 transformation, VkSampler imgSampler);

    /**
     * @brief Adds a test object to the buffer manager
     * @param id of the object
     * @param vertices
     * @param indices
     * @param transformation
     * @return boolean indicating success
     */
    bool addTestObject(int id, const std::vector<SimpleVertex>& vertices, const std::vector<uint32_t>& indices, glm::mat4 transformation);

    /**
     * @brief Adds an area object to the buffer manager
     * @param vertices of the area
     * @param indices of the area
     * @param textures of the area
     * @return boolean indicating success
     */
    bool addArea(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::array<ImageData, 6> textures, VkSampler texSampler);

    /**
     * @brief Updates test object data.
     * @param id is the ID of the object.
     * @param srcData is the source data.
     * @param frame is the frame number.
     */
    void updateTestObject(int id, const void* srcData, uint32_t frame);

    /**
     * @brief Updates the object descriptor with the given data.
     * @param id of the object to update
     * @param srcData is the data to be copied to the buffer (4x4 matrix)
     * @param frame is the frame of the update
     */
    void updateObjectDescriptor(int id, const void* srcData, uint32_t frame);

    /**
     * @brief
     * @param id of the character
     * @param transformSrcData is the transformation data to be copied to the buffer (4x4 matrix)
     * @param boneSrcData is the bone data to be copied (list of matrixes)
     * @param frame is the frame of the update
     */
    void updateCharacterDescriptor(int id, const void* transformSrcData, const void* boneSrcData, uint32_t frame);

    /**
     * @brief Creates a depth image.
     * @param width is the width of the image.
     * @param height is the height of the image.
     * @param depthFormat is the format of the depth image.
     * @returns AllocatedImage object.
     */
    AllocatedImage createDepthImage(uint32_t width, uint32_t height, VkFormat depthFormat);

    /**
     * @brief Interface for destroying an image That is owned by the user of this class.
     * @param image
     */
    void destroyImage(AllocatedImage image);

    /**
     * @brief Transitions the given image to the given layout using graphics queue.
     * @param depthImage
     * @param depthFormat
     * @param srcLayout
     * @param dstLayout
     */
    void transitionDepthImageLayout(AllocatedImage depthImage, VkFormat depthFormat, VkImageLayout srcLayout, VkImageLayout dstLayout);

    /**
     * @brief Creates an image view for the given image and format.
     * @param image The image for which to create a view.
     * @param format The format of the image view.
     * @return The created image view. The caller is responsible for the destruction of the allocated resource.
     */
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

    /**
     * @brief Creates an image for the given data and stores the memory to this class
     * @param data of the image
     * @return combined image struct (image allocation, image view and vkimage)
     */
    CombinedImage createCombinedImage(uint32_t width, uint32_t height, VkFormat format);

    /**
     * @brief Copies the given data to the given buffer.
     * @param allocBuffer is the buffer to which the data is copied.
     * @param srcData is the source data.
     */
    void copyBuffer(AllocatedBuffer allocBuffer, const void* srcData);

    /**
     * @brief Copies the given data to the given image.
     * @param allocImage is the image to copy to.
     * @param srcData is the source image data.
     */
    void copyImage(AllocatedImage allocImage, const ImageData& srcData);

    /**
     * @brief Returns the descriptor set layout for the given object type.
     * @param type
     * @return
     */
    VkDescriptorSetLayout getLayout(ObjectType type) const;

    /**
     * @brief Returns the number of descriptor layouts of the given type. Currently not a very useful function. However, it might be useful in the future.
     * @param type of object to be queried for.
     * @return number of layouts
     */
    uint32_t getLayoutCount(ObjectType type) const;

    /**
     * @brief Returns drawable objects of the given type.
     * @param type of object desired for drawing
     * @return List of drawableobject structs
     */
    std::vector<DrawableObject> getDrawableObjects(ObjectType type, uint32_t frame) const;

private:
    /**
     * @brief Command unit represents everything needed to allocate, submit, record and execute commands.
     */
    struct CommandUnit
    {
        VkQueue queue;             ///< Queue for the command unit
        VkCommandPool commandPool; ///< Command pool for the command unit
        uint32_t queueFamilyIndex; ///< Queue family index of the command unit
        VkCommandBuffer startCommandBuffer(VkDevice logDevice);
        bool endCommandBuffer(VkDevice logDevice, VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE);
    };

    /**
     * @brief Creates a buffer with the given parameters.
     * @param size is the size of the buffer in bytes.
     * @param usage is the usage of the buffer.
     * @param info is the allocation info of the buffer to be filled.
     * @param memUsage is the memory usage of the buffer.
     * @param vmaCreationFlags are the flags for the creation of the buffer.
     * @param requiredFlags are the required memory property flags.
     * @param preferredFlags are the preferred memory property flags.
     * @return The allocated buffer.
     */
    AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationInfo& info, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);
    AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

    /**
     * @brief Creates an allocated image.
     *        The standard parameters make it easy to create a texture image on the GPU.
     *        More specific use cases must be specified through the parameters.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param format The format of the image.
     * @param tiling The tiling of the image.
     * @param usage The usage of the image.
     * @param memUsage The VMA memory usage of the image.
     * @param vmaCreationFlags The VMA creation flags of the image.
     * @param requiredFlags The required memory property flags.
     * @param preferredFlags The preferred memory property flags.
     * @return The allocated image.
     */
    AllocatedImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

    /**
     * @brief Initializes the descriptorBuilder member.
     */
    void createDescriptorBuilder();

    /**
     * @brief Returns number of queue families
     * @return
     */
    uint32_t getQueueFamilyCount();

    /**
     * @brief Returns the sharing mode of created resources.
     * @param id of the object
     * @return VK_SHARING_MODE_CONCURRENT if separate graphics queue is set.
     * @return VK_SHARING_MODE_EXCLUSIVE if transfer queue is the graphics queue.
     */
    VkSharingMode getSharingMode();

    /**
     * @brief Allocates and begins a command buffer.
     * @param preferTransfer indicates if the transfer unit should be used.
     * @return The allocated command buffer.
     */
    VkCommandBuffer startCommandBuffer(bool preferTransfer);

    /**
     * @brief Submits the given command buffer to be executed on the gpu.
     * @param commandBuffer to end
     * @param fence to set signal.
     * @return boolean indicating success
     */
    bool endCommandBuffer(VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE);

    /**
     * @brief Records image layout transition barrier to the given command buffer.
     * @param image to transition
     * @param format of the image
     * @param oldLayout of the image
     * @param newLayout of the image
     * @param commandBuffer to record the transition to
     * @param srcQueueFamilyIndex & dstQueueFamilyIndex of the source family. Use the default value if you do not want to change queue ownership
     */
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, std::pair<uint32_t, uint32_t> srcAndDstQueueFamilies = {VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED});

    /**
     * @brief Queue families should be transferred, if the resources are using multiple queues, and the sharingmode is exclusive, or the desired stage is available only to another queue.
     * todo could be updated in the future.
     */
    std::pair<uint32_t, uint32_t> getQueueTransitionFamilies();

    /**
     * @brief Fills access masks and pipeline stages when given source and destination layouts for an image.
     * @param srcLayout of the image
     * @param dstLayout of the image
     * @param srcAndDstAccessMasks to be filled
     * @param srcAndDstStages to be filled
     * @return boolean indicating if the given layout pair is valid
     */
    bool getPipelineAndAccessFlags(VkImageLayout srcLayout, VkImageLayout dstLayout, std::pair<VkAccessFlags, VkAccessFlags>& srcAndDstAccessMasks, std::pair<VkPipelineStageFlags, VkPipelineStageFlags>& srcAndDstStages);

    /**
     * @brief Helper for creating a staging buffer.
     * @param size of the staging buffer
     * @param allocInfo to be filled
     * @return The allocated buffer
     */
    AllocatedBuffer createStagingBuffer(VkDeviceSize size, VmaAllocationInfo& allocInfo);

    // Required Vulkan handles
    VkDevice logDeviceM;
    VmaAllocator allocatorM;

    std::shared_mutex& graphicsQueueMutexM;
    CommandUnit graphicsCommandUnitM;
    std::optional<CommandUnit> transferCommandUnitM;
    // Todo make a better system
    std::optional<CommandUnit> activeCommandUnitM;
    // Builder
    std::unique_ptr<DescriptorBuilder> pBuilderM;
    // Data maps
    std::unordered_map<int, CharacterObject> characterObjectsM;
    std::unordered_map<int, GameObject> gameObjectsM;
    std::unordered_map<int, TestObject> testObjectsM;
    std::vector<AreaObject> areaObjectsM;

    std::vector<CombinedImage> allocatedImagesM;

    // Frame count
    uint32_t frameCountM;
    // Descriptor set layouts for common objects
    std::array<VkDescriptorSetLayout, OBJECT_TYPE_COUNT> descriptorSetLayoutsM;
};
