#include "ShaderManager.hpp"

#include <fstream>
#include <iostream>

#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/intermediate.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

// Shader sources
#define CPP_INCLUDE_SHADERS
#include "characterShader.vert"
#include "testShader.vert"
#include "objectShader.vert"
#include "mapShader.vert"

#include "characterShader.frag"
#include "objectShader.frag"
#include "testShader.frag"
#include "mapShader.frag"

std::array<std::string, OBJECT_TYPE_COUNT> vertexShaderCodes{ characterVertexShaderSource, objectVertexShaderSource, testVertexShaderSource, mapVertexShaderSource };
std::array<std::string, OBJECT_TYPE_COUNT> fragmentShaderCodes{ characterFragmentShaderSource, objectFragmentShaderSource, testFragmentShaderSource, mapFragmentShaderSource };

VkShaderModule ShaderManager::createShaderModule(VkDevice logDevice, VkShaderStageFlagBits stage, ObjectType objectType)
{
	VkShaderModuleCreateInfo shaderInfo{};
	std::vector<uint32_t> spirvCode = ShaderManager::compileGLSLToSpirv(stage, objectType);
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.codeSize = spirvCode.size() * sizeof(uint32_t); // size in bytes, however, I don't like this.
	shaderInfo.pCode = spirvCode.data();

	VkShaderModule module;
	if (vkCreateShaderModule(logDevice, &shaderInfo, nullptr, &module) != VK_SUCCESS)
	{
		throw std::runtime_error("Shadermodule creation failed");
	}
	// add module to createdModules to be able to destroy them later
	createdModules.push_back(module);

	return module;
}

VkPipelineShaderStageCreateInfo ShaderManager::createVertexShaderInfo(VkDevice logDeviceP, ObjectType objectType)
{
	VkPipelineShaderStageCreateInfo shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderInfo.module = createShaderModule(logDeviceP, shaderInfo.stage, objectType);
	shaderInfo.pName = "main";  // Specifies the entrypoint. You can combine multiple shaders with different entrypoints and pick the one you want. 

	return shaderInfo;
}

VkPipelineShaderStageCreateInfo ShaderManager::createFragmentShaderInfo(VkDevice logDeviceP, ObjectType objectType)
{
	VkPipelineShaderStageCreateInfo shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderInfo.module = createShaderModule(logDeviceP, shaderInfo.stage, objectType);
	shaderInfo.pName = "main";  // Specifies the entrypoint. You can combine multiple shaders with different entrypoints and pick the one you want. 

	return shaderInfo;
}

void ShaderManager::destroyShaderModules(VkDevice logDevice)
{
	for (auto& module : createdModules)
	{
		vkDestroyShaderModule(logDevice, module, nullptr);
	}
	createdModules.clear();
	glslang::FinalizeProcess(); // let's see if this works
}

std::vector<char> ShaderManager::readFile(ConstantFilepath pFilename)
{
	std::ifstream fileStream(pFilename, std::ios::ate | std::ios::binary);

	if (!fileStream.is_open())
	{
		throw std::runtime_error("Opening a file failed");
	}

	size_t fileSize = (size_t)fileStream.tellg();
	std::vector<char> buffer(fileSize);

	fileStream.seekg(0); // seek end of file
	fileStream.read(buffer.data(), fileSize);

	fileStream.close();
	return buffer;
}


std::vector<uint32_t> ShaderManager::compileGLSLToSpirv(VkShaderStageFlagBits shaderType, ObjectType objectType)
{
	glslang::InitializeProcess(); // Initialize glslang compiler, process is finalized when modules are destroyed.
	std::vector<uint32_t> spirv;
	std::string glslCode;
	EShLanguage stage;
	switch (shaderType) // Find the shader type
	{
		case VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT:
			stage = EShLangVertex;
			break;
		case VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT:
			stage = EShLangFragment;
			break;
		default: // Currently only supporting 2 types of shaders.
			throw std::runtime_error("Shader type not supported");
	}
	// Shader code can now be fetched using a single function call.
	glslCode = getShaderCode(shaderType, objectType);

	const char* pShaderCode = glslCode.c_str();
	glslang::TShader shader(stage);
	shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	shader.setSourceEntryPoint(ShaderManager::kStandardEntryPoint);
	shader.setEntryPoint(ShaderManager::kStandardEntryPoint);

	glslang::TProgram program;
	const TBuiltInResource* kpResources = GetDefaultResources();
	EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

	shader.setStrings(&pShaderCode, 1);
	// Version is an int. Is it the glsl version number?
	// Does not seem like it, at least according to their own examples.
	if (!shader.parse(kpResources, 100, false, messages))
	{
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		fflush(stdout);
		throw std::runtime_error("GLSL shader parsing failed");
	}
	// If parsing succeeded, add shader
	program.addShader(&shader);
	if (!program.link(messages))
	{
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		fflush(stdout);
		throw std::runtime_error("GLSL shader linking failed");
	}

	glslang::TIntermediate* intermediate = program.getIntermediate(stage);
	glslang::SpvOptions spvOptions{};
	spvOptions.generateDebugInfo = true;
	spvOptions.disableOptimizer = true;

	glslang::GlslangToSpv(*intermediate, spirv, &spvOptions);
	return spirv;
}

std::string ShaderManager::getShaderCode(VkShaderStageFlags stage, ObjectType object)
{
	std::string code;
	if (stage == VK_SHADER_STAGE_VERTEX_BIT)
	{
		code = vertexShaderCodes[object];
	}
	else if (stage == VK_SHADER_STAGE_FRAGMENT_BIT)
	{
		code = fragmentShaderCodes[object];
	}
	else
	{
		throw std::runtime_error("Unsupported shader stage requested!");
	}
	return code;
}

