#pragma once
#include <vk_mem_alloc.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <array>
#include <memory>

#pragma region Constants
constexpr uint32_t BONES_PER_VERTEX = 4;
constexpr uint32_t MAX_BONES = 50;

constexpr glm::vec3 GAMEOBJECT_MIN = glm::vec3(-0.5f, -0.5f, -0.5f);
constexpr glm::vec3 GAMEOBJECT_MAX = glm::vec3(0.5f, 0.5f, 0.5f);

constexpr size_t OBJECT_TYPE_COUNT = 4;
constexpr size_t ANIMATION_TYPE_COUNT = 5;

enum ObjectType : uint32_t
{
	CHARACTER,
	GAMEOBJECT,
	TESTOBJECT,
	AREA,
	UNDEFINED
};

enum CharacterAnimation : uint32_t
{
	IDLE,
	WALK,
	RUN,
	ATTACK,
	DEATH,
};

std::array<ObjectType, OBJECT_TYPE_COUNT> getObjectTypes();

std::array <CharacterAnimation, ANIMATION_TYPE_COUNT> getAnimationTypes();

#pragma endregion

#pragma region GraphicsObjectTypes

struct Hit
{
	int id;
	ObjectType objectType;
	glm::vec3 hitPoint;
};

struct ImageData
{
	unsigned char* pixels;
	int width;
	int height;
};

struct DrawableObject
{
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
	VkDescriptorSet descriptorSet;
};

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
	VkDeviceSize size;
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
	VkDescriptorSet descriptorSet;	 // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	void* mappedTransformData;			 // Pointer to the mapped data of the transform buffer
};

// Test object descriptor data
struct TestObjectUniformBuffer
{
	VkDescriptorSet descriptorSet;	 // Descriptor set of the data
	AllocatedBuffer transformBuffer; // Buffer containing the transform data (glm::mat4)
	void* mappedTransformData;			 // Pointer to the mapped data of the transform buffer
};

// Character buffer object
struct CharacterObjectUniformBuffer
{
	VkDescriptorSet descriptorSet;			 // Descriptor set of the data
	AllocatedBuffer boneTransformations; // Todo think how the data is going to be stored
	AllocatedBuffer boneWeights;
};

// Object that contains everything needed to render a character.
struct CharacterObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	VkImageView textureView;
	std::vector<CharacterObjectUniformBuffer> characterUniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 2> getDescriptorSetLayoutBindings();
};

struct GameObject
{
	AllocatedBuffer vertexData;
	AllocatedBuffer indexData;
	uint32_t indexCount;
	AllocatedImage texture;
	VkImageView textureView;
	std::vector<GameObjectUniformBuffer> uniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 2> getDescriptorSetLayoutBindings();
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
	std::vector<TestObjectUniformBuffer> uniformBuffers;
	static std::array<VkDescriptorSetLayoutBinding, 1> getDescriptorSetLayoutBindings();
};

#pragma endregion

#pragma region VertexTypes

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

struct CharacterVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::uvec4 boneIDs;
	glm::vec4 boneWeights;
	glm::vec2 texCoord;
	// Vulkan binding and attribute descriptions
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	// Vulkan binding and attribute descriptions
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

// TODO Use assimp and checkout the library.
struct SimpleVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	// Vulkan binding and attribute descriptions
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

#pragma endregion