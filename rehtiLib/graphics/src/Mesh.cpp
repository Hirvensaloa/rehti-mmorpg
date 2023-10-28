#include "Mesh.hpp"

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

VkDescriptorSetLayoutBinding Transformation::getDescriptorSetBinding()
{
	// Using single binding for desc set layout to be used in vertex shader
	// Sort of similar to vertex input binding description
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = 0;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	binding.pImmutableSamplers = nullptr; // Optional, possibly to be added later

	return binding;
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

std::vector<SimpleVertex> createCubeVertices()
{
	std::vector<SimpleVertex> vertices;

	// Define the 8 vertices of the cube with positions and colors
	vertices.push_back({ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f) }); // Vertex 0
	vertices.push_back({ glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });  // Vertex 1
	vertices.push_back({ glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f) });  // Vertex 2
	vertices.push_back({ glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) }); // Vertex 3
	vertices.push_back({ glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f) }); // Vertex 4
	vertices.push_back({ glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f) });  // Vertex 5
	vertices.push_back({ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f) });   // Vertex 6
	vertices.push_back({ glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.0f, 0.5f) });   // Vertex 7

	return vertices;
}

std::vector<uint32_t> createCubeIndices()
{
	std::vector<uint32_t> indices;

	// Front face
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	// Back face
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(4);

	// Top face
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(3);

	// Bottom face
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(5);
	indices.push_back(4);
	indices.push_back(0);

	// Left face
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(4);
	indices.push_back(0);

	// Right face
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(1);

	return indices;
}