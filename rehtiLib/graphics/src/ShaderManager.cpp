#include "ShaderManager.hpp"

#include <fstream>

VkShaderModule ShaderManager::createShaderModule(VkDevice logDeviceP, ConstantFilepath filepathP)
{
    VkShaderModuleCreateInfo shaderInfo{};
    std::vector<char> code = ShaderManager::readFile(filepathP);
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(logDeviceP, &shaderInfo, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("Shadermodule creation failed");
    }

    return module;
}

VkPipelineShaderStageCreateInfo ShaderManager::createVertexShaderInfo(VkDevice logDeviceP)
{
    VkPipelineShaderStageCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderInfo.module = createShaderModule(logDeviceP, kVertexShaderPath);
    shaderInfo.pName = "main";  //specifies the entrypoint. You can combine multiple shaders with different entrypoints and pick the one you want. 

}

VkPipelineShaderStageCreateInfo ShaderManager::createFragmentShaderInfo(VkDevice logDeviceP)
{
    VkPipelineShaderStageCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderInfo.module = createShaderModule(logDeviceP, kFragmentShaderPath);
    shaderInfo.pName = "main";  //specifies the entrypoint. You can combine multiple shaders with different entrypoints and pick the one you want. 

}

std::vector<char> ShaderManager::readFile(ConstantFilepath pFilenameP)
{
    std::ifstream fileStream(pFilenameP, std::ios::ate | std::ios::binary);

    if (!fileStream.is_open()) 
    {
        throw std::runtime_error("Opening a file failed");
    }

    size_t fileSize = (size_t) fileStream.tellg(); 
    std::vector<char> buffer(fileSize);

    fileStream.seekg(0); // seek end of file
    fileStream.read(buffer.data(), fileSize);

    fileStream.close();
    return buffer;
}
