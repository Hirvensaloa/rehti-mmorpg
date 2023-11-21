#include "GraphicsTypes.hpp"

#include <glm/ext.hpp>
#include <glm/ext/quaternion_common.hpp>

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

std::array<VkDescriptorSetLayoutBinding, 3> CharacterObject::getDescriptorSetLayoutBindings()
{
    std::array<VkDescriptorSetLayoutBinding, 3> array;
    // basic transformation
    array[0].binding = 0;
    array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    array[0].descriptorCount = 1;
    array[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    array[0].pImmutableSamplers = nullptr;

    // transformations
    array[1].binding = 1;
    array[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    array[1].descriptorCount = MAX_BONES;
    array[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    array[1].pImmutableSamplers = nullptr;

    array[2].binding = 2;
    array[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    array[2].descriptorCount = 1;
    array[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    array[2].pImmutableSamplers = nullptr;

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

std::array<VkDescriptorSetLayoutBinding, 1> AreaObject::getDescriptorSetLayoutBindings()
{
    std::array<VkDescriptorSetLayoutBinding, 1> array;

    array[0].binding = 0;
    array[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    array[0].descriptorCount = 6; // 6 textures
    array[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    array[0].pImmutableSamplers = nullptr;

    return array;
}

std::array<ObjectType, OBJECT_TYPE_COUNT> getObjectTypes()
{
    std::array<ObjectType, OBJECT_TYPE_COUNT> array;
    for (uint32_t type = 0; type < OBJECT_TYPE_COUNT; type++)
    {
        array[type] = static_cast<ObjectType>(type);
    }
    return array;
}

std::array<AnimationType, ANIMATION_TYPE_COUNT> getAnimationTypes()
{
    std::array<AnimationType, ANIMATION_TYPE_COUNT> array;
    for (uint32_t anim = 0; anim < ANIMATION_TYPE_COUNT; anim++)
    {
        array[anim] = static_cast<AnimationType>(anim);
    }
    return array;
}

glm::mat4 AnimationNode::getTransformationMatrix() const
{
    glm::mat4 transformation = glm::mat4(1.0f);

    transformation = glm::scale(transformation, scale);
    transformation = glm::mat4(rotation) * transformation;
    transformation = glm::translate(transformation, position);
    return transformation;
}

AnimationNode AnimationNode::interpolate(AnimationNode first, AnimationNode second, float normalizedTime)
{
    AnimationNode interpolatedNode{};
    float normalizedTimeClamped = glm::clamp(normalizedTime, 0.0f, 1.0f);
    float inverseWeight = 1.f - normalizedTimeClamped;
    interpolatedNode.position = first.position * inverseWeight + second.position * normalizedTimeClamped;
    interpolatedNode.scale = first.scale * inverseWeight + second.scale * normalizedTimeClamped;
    interpolatedNode.rotation = glm::slerp(first.rotation, second.rotation, normalizedTimeClamped);
    return interpolatedNode;
}

VkVertexInputBindingDescription getSimpleVertexBindingDescription()
{
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(SimpleVertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 2> getSimpleVertexAttributeDescriptions()
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

VkVertexInputBindingDescription getVertexBindingDescription()
{
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions()
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

VkVertexInputBindingDescription getCharacterVertexBindingDescription()
{
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(CharacterVertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 5> getCharacterVertexAttributeDescriptions()
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

std::vector<VkVertexInputAttributeDescription> getAttributeDescription(ObjectType objectType)
{
    std::vector<VkVertexInputAttributeDescription> attributeDescs{};
    switch (objectType)
    {
    case ObjectType::CHARACTER:
        for (auto desc : getCharacterVertexAttributeDescriptions())
        {
            attributeDescs.push_back(desc);
        }
        break;
    case ObjectType::GAMEOBJECT:
        for (auto desc : getVertexAttributeDescriptions())
        {
            attributeDescs.push_back(desc);
        }
        break;
    case ObjectType::TESTOBJECT:
        for (auto desc : getSimpleVertexAttributeDescriptions())
        {
            attributeDescs.push_back(desc);
        }
        break;
    case ObjectType::AREA:
        for (auto desc : getVertexAttributeDescriptions())
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
        desc = getCharacterVertexBindingDescription();
        break;
    case ObjectType::GAMEOBJECT:
        desc = getVertexBindingDescription();
        break;
    case ObjectType::TESTOBJECT:
        desc = getSimpleVertexBindingDescription();
        break;
    case ObjectType::AREA:
        desc = getVertexBindingDescription();
        break;
    default:
        break;
    }
    return desc;
}
