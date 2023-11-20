#pragma once
#include "DescriptorBuilder.hpp"
#include "Mesh.hpp"
#include "GraphicsTypes.hpp"

#include <vector>
#include <unordered_map>
#include <array>
#include <optional>
#include <utility>

/// <summary>
/// Graphics object manager manages different types of objects that have memory and can be drawn.
/// </summary>
class GraphicsObjectManager
{
public:
	/// <summary>
	/// Creates a graphics object manager.
	/// </summary>
	/// <param name="instance"> is a vulkan instance.</param>
	/// <param name="gpu"></param>
	/// <param name="logDevice"></param>
	/// <param name="transferQueue"> that can receive transfer operations.</param>
	/// <param name="queueFamily"> of the queue that was supplied</param>
	GraphicsObjectManager(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, VkQueue graphicsQueue, uint32_t graphicsQueueFamily, const uint32_t frameCount);

	/// <summary>
	/// Destructor
	/// </summary>
	~GraphicsObjectManager();

	/// <summary>
	/// Adds the given transfer queue to the manager.
	/// </summary>
	/// <param name="families"></param>
	/// <param name="familyCount"></param>
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
	bool addCharacter(int characterID, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, ImageData& texture, glm::mat4 transformation, glm::mat4 bindPose[MAX_BONES], VkSampler imgSampler);

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


	/// <summary>
	/// Updates test object data
	/// </summary>
	/// <param name="id"></param>
	/// <param name="srcData"></param>
	/// <param name="frame"></param>
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

	/// <summary>
	/// Creates a depth image
	/// </summary>
	/// <returns></returns>
	AllocatedImage createDepthImage(uint32_t width, uint32_t height, VkFormat depthFormat);

	/**
	 * @brief Interface for destroying an image.
	 * TODO redo. If the idea is to let this class manage all the resources, then let it do just that.
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

	/// <summary>
	/// Creates an image view for the given image and format.
	/// Todo extend support for more parameters and configurations.
	/// </summary>
	/// <param name="image"> to create a view for.</param>
	/// <param name="format"> of the image view.</param>
	/// <returns>The created image view. The caller is responsible for the destruction of the allocated resource.</returns>
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	/// <summary>
	/// Copies the given data to the given buffer.
	/// </summary>
	/// <param name="allocBuffer"> is the allocated buffer struct</param>
	/// <param name="srcData"> is the data to cpy</param>
	void copyBuffer(AllocatedBuffer allocBuffer, const void* srcData);

	/// <summary>
	/// Copies image data to the given image.
	/// </summary>
	/// <param name="allocImage"></param> 
	/// <param name="srcData"></param>
	void copyImage(AllocatedImage allocImage, const ImageData& srcData);

	/// <summary>
	/// Returns the layout of the given type.
	/// </summary>
	/// <param name="type"> of object for which a layout is desired</param>
	/// <returns>Descriptor set layout</returns>
	VkDescriptorSetLayout getLayout(ObjectType type) const;

	/**
	 * @brief Returns the number of descriptor layouts of the given type. Currently not a very useful function. However, it might be useful in the future.
	 * @param type of object to be queried for.
	 * @return number of layouts
	*/
	uint32_t getLayoutCount(ObjectType type) const;

	/// <summary>
	/// Returns drawable objects of the given type.
	/// </summary>
	/// <param name="type"> of object desired for drawing</param>
	/// <returns>List of drawableobject structs</returns>
	std::vector<DrawableObject> getDrawableObjects(ObjectType type, uint32_t frame) const;

private:
	/**
	 * @brief Command unit represents everything needed to allocate, submit, record and execute commands.
	*/
	struct CommandUnit
	{
		VkQueue queue;
		VkCommandPool commandPool;
		uint32_t queueFamilyIndex;
		VkCommandBuffer startCommandBuffer(VkDevice logDevice);
		bool endCommandBuffer(VkDevice logDevice, VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE);
	};

	/// <summary>
	/// Creates a buffer with the given parameters.
	/// </summary>
	/// <param name="size">Size of the buffer in bytes.</param>
	/// <param name="usage">Flags signaling the usage of the buffer to be allocated.</param>
	AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationInfo& info, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);
	AllocatedBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

	/// <summary>
	/// Creates an allocated image. The standard parameters make it easy to create a texture image on the gpu.
	/// More specific use case must be specified through the parameters.
	/// </summary>
	/// <param name="width">of the image</param>
	/// <param name="height"> of the image</param>
	/// <param name="format"> of the image</param>
	/// <param name="tiling"> of the image</param>
	/// <param name="usage"> of the image</param>
	/// <param name="memUsage"> is the vma memory usage of the image</param>
	AllocatedImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaCreationFlags = 0, VkMemoryPropertyFlags requiredFlags = 0, VkMemoryPropertyFlags preferredFlags = 0);

	/// <summary>
	/// Initiates the descriptor builder.
	/// </summary>
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
	 * @brief
	 * @return
	*/
	VkCommandBuffer startCommandBuffer(bool preferTransfer);

	/**
	 * @brief Tries to end the given command buffer.
	 * @param commandBuffer
	 * @param fence
	 * @return
	*/
	bool endCommandBuffer(VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE);

	/**
	 * @brief Records image layout transition barrier to the given command buffer.
	 * @param image
	 * @param format
	 * @param oldLayout
	 * @param newLayout
	 * @param commandBuffer
	 * @param srcQueueFamilyIndex & dstQueueFamilyIndex of the source family. Use the default value if you do not want to change queue ownership
	*/
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, std::pair<uint32_t, uint32_t> srcAndDstQueueFamilies = { VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED });

	/**
	 * @brief Queue families should be transferred, if the resources are using multiple queues, and the sharingmode is exclusive, or the desired stage is available only to another queue.
	 * TODO REWORK TRANSITIONS
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

	// Frame count
	uint32_t frameCountM;
	// Descriptor set layouts for common objects
	std::array<VkDescriptorSetLayout, OBJECT_TYPE_COUNT> descriptorSetLayoutsM;
};
