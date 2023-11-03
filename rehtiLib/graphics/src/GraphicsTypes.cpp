#include "GraphicsTypes.hpp"

VkVertexInputBindingDescription SimpleVertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.stride = sizeof(SimpleVertex);
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 2> SimpleVertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescs{};
	attributeDescs[0].binding = 0;
	attributeDescs[0].location = 0;
	attributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[0].offset = offsetof(SimpleVertex, pos);

	attributeDescs[1].binding = 0; // one binding desc for all attributes
	attributeDescs[1].location = 1;
	attributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[1].offset = offsetof(SimpleVertex, color);

	return attributeDescs;
}

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.stride = sizeof(Vertex);
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescs{};
	attributeDescs[0].binding = 0;
	attributeDescs[0].location = 0;
	attributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[0].offset = offsetof(Vertex, pos);

	attributeDescs[1].binding = 0; // one binding desc for all attributes
	attributeDescs[1].location = 1;
	attributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[1].offset = offsetof(Vertex, normal);

	attributeDescs[2].binding = 0;
	attributeDescs[2].location = 2;
	attributeDescs[2].format = VK_FORMAT_R32G32_SFLOAT; // 2 floats
	attributeDescs[2].offset = offsetof(Vertex, texCoord);
	return attributeDescs;
}

VkVertexInputBindingDescription CharacterVertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.stride = sizeof(CharacterVertex);
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 5> CharacterVertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 5> attributeDescs{};

	attributeDescs[0].binding = 0;
	attributeDescs[0].location = 0;
	attributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[0].offset = offsetof(CharacterVertex, pos);

	attributeDescs[1].binding = 0;
	attributeDescs[1].location = 1;
	attributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
	attributeDescs[1].offset = offsetof(CharacterVertex, normal);

	attributeDescs[2].binding = 0;
	attributeDescs[2].location = 2;
	attributeDescs[2].format = VK_FORMAT_R32G32B32A32_UINT; // 4 uint32_t
	attributeDescs[2].offset = offsetof(CharacterVertex, boneIDs);

	attributeDescs[3].binding = 0;
	attributeDescs[3].location = 3;
	attributeDescs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT; // 4 floats
	attributeDescs[3].offset = offsetof(CharacterVertex, boneWeights);

	attributeDescs[4].binding = 0;
	attributeDescs[4].location = 4;
	attributeDescs[4].format = VK_FORMAT_R32G32_SFLOAT; // 2 floats
	attributeDescs[4].offset = offsetof(Vertex, texCoord);

	return attributeDescs;
}


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

std::array<VkDescriptorSetLayoutBinding, 2> CharacterObject::getDescriptorSetLayoutBindings()
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

std::array<VkDescriptorSetLayoutBinding, 2> GameObject::getDescriptorSetLayoutBindings()
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

std::array<ObjectType, OBJECT_TYPE_COUNT> getObjectTypes()
{
	return { CHARACTER, GAMEOBJECT, TESTOBJECT };
}

std::vector< VkVertexInputAttributeDescription> getAttributeDescription(ObjectType objectType)
{
	std::vector< VkVertexInputAttributeDescription> attributeDescs{};
	switch (objectType)
	{
		case ObjectType::CHARACTER:
			for (auto desc : CharacterVertex::getAttributeDescriptions())
			{
				attributeDescs.push_back(desc);
			}
			break;
		case ObjectType::GAMEOBJECT:
			for (auto desc : Vertex::getAttributeDescriptions())
			{
				attributeDescs.push_back(desc);
			}
			break;
		case ObjectType::TESTOBJECT:
			for (auto desc : SimpleVertex::getAttributeDescriptions())
			{
				attributeDescs.push_back(desc);
			}
			break;
		default:
			break;
	}

	return attributeDescs;
}

VkVertexInputBindingDescription getBindingDescription(ObjectType objectType)
{
	VkVertexInputBindingDescription desc{};
	switch (objectType)
	{
		case ObjectType::CHARACTER:
			desc = CharacterVertex::getBindingDescription();
			break;
		case ObjectType::GAMEOBJECT:
			desc = Vertex::getBindingDescription();
			break;
		case ObjectType::TESTOBJECT:
			desc = SimpleVertex::getBindingDescription();
			break;
		default:
			break;
	}
	return desc;
}

bool AABB::isLeaf()
{
	return pLeft == nullptr && pRight == nullptr;
}

glm::vec3 AABB::getCenter()
{
	return 0.5f * min + 0.5f * max;
}
