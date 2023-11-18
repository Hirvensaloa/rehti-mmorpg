#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <array>
#include <memory>

#ifndef DEFINE_EXPLICIT_TYPES
#define ALLOCATED_TYPES
#define VK_FUNCTIONS
#endif

#pragma region Constants

constexpr uint32_t BONES_PER_VERTEX = 4;
constexpr uint32_t MAX_BONES = 50;

constexpr glm::vec3 GAMEOBJECT_MIN = glm::vec3(-0.5f, -0.5f, -0.5f);
constexpr glm::vec3 GAMEOBJECT_MAX = glm::vec3(0.5f, 0.5f, 0.5f);

constexpr glm::vec3 CHARACTER_MIN = glm::vec3(-0.5f, 0.0f, -0.5f);
constexpr glm::vec3 CHARACTER_MAX = glm::vec3(0.5f, 2.0f, 0.5f);

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

enum AnimationType : uint32_t
{
	IDLE,
	WALK,
	RUN,
	ATTACK,
	DEATH,
};

std::array<ObjectType, OBJECT_TYPE_COUNT> getObjectTypes();

std::array <AnimationType, ANIMATION_TYPE_COUNT> getAnimationTypes();

#pragma endregion

#pragma region AnimationTypes
// Animation node. The data stored can also represent any kind of transformation.
struct GfxOrientation
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 getTransformationMatrix() const;
	static GfxOrientation interpolate(GfxOrientation first, GfxOrientation second, float factor);
};

struct AnimationNode
{
	double time; // time of this animation node in ticks
	std::array<GfxOrientation, MAX_BONES> bones;
};

// Immutable data. Animations should be stored somewhere and requested when needed to be stored for a character.
struct Animation
{
	double totalTicks; // total ticks in the animation
	double ticksPerSecond; // ticks per second
	float duration; // duration of the animation in seconds
	std::vector<AnimationNode> animationNodes; // animation nodes
};

struct CharacterAnimationData
{
	AnimationType currentAnimation;
	double currentTicks;
	std::array<Animation, ANIMATION_TYPE_COUNT> animations;
};

struct BoneNode
{
	int parent; // index of the parent in bone array.
	std::vector<uint32_t> children; // indices of the children in bone array.
};

struct CharacterData
{
	GfxOrientation characterOrientation; // orientation of the character
	std::array<glm::mat4, MAX_BONES> boneTransformations{}; // bone transformation storage data
	std::vector<BoneNode> bones;
	CharacterAnimationData animationData;
	void advanceAnimation(float dt);
};

#pragma endregion


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


#pragma region GraphicsObjectTypes

#ifdef ALLOCATED_TYPES
#include <vk_mem_alloc.h>

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
	AllocatedBuffer transformBuffer;		 // Buffer containing model to world matrix
	AllocatedBuffer boneTransformations;	 // Buffer containing bone transformations
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
	static std::array<VkDescriptorSetLayoutBinding, 3> getDescriptorSetLayoutBindings();
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

#endif

#pragma endregion

#pragma region VertexTypes


struct CharacterVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::uvec4 boneIDs;
	glm::vec4 boneWeights;
	glm::vec2 texCoord;

#ifdef VK_FUNCTIONS
#include <vulkan/vulkan.h>
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
#endif
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
#ifdef VK_FUNCTIONS
	// Vulkan binding and attribute descriptions
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
#endif
};

// TODO Use assimp and checkout the library.
struct SimpleVertex
{
	glm::vec3 pos;
	glm::vec3 color;
#ifdef VK_FUNCTIONS

	// Vulkan binding and attribute descriptions
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
#endif
};


#ifdef VK_FUNCTIONS
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

#endif

#pragma endregion