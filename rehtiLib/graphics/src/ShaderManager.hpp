#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <string>

typedef char const * const ConstantFilepath;

namespace ShaderManager
{
	// Constants
	constexpr ConstantFilepath kGLSLVertexShaderPath = "shaders/shader.vert";
	constexpr ConstantFilepath kGLSLFragmentShaderPath = "shaders/shader.frag";
	constexpr const char* kStandardEntryPoint = "main";

	/// <summary>
	/// Creates a vertex shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(VkDevice logDevice);

	/// <summary>
	/// Creates a fragment shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(VkDevice logDevice);

	/// <summary>
	/// Destroys created shader modules. This function can be called after pipeline creation.
	/// </summary>
	/// <param name="logDevice">The logical device.</param>
	void destroyShaderModules(VkDevice logDevice);

	/// <summary>
	/// Compiles a given GLSL code to SPIRV.
	/// </summary>
	/// <param name="glslCode">GLSL code as a string</param>
	/// <param name="shaderType">Shader type enum</param>
	/// <returns>Compiled SPIRV aligned to 4 bytes.</returns>
	std::vector<uint32_t> compileGLSLToSpirv(VkShaderStageFlagBits shaderType);

	/// <summary>
	/// Reads a file and returns a vector of chars.
	/// </summary>
	/// <param name="rFilename">Name of the file to read.</param>
	/// <returns>A vector of characters read from the file.</returns>
	static std::vector<char> readFile(ConstantFilepath filepath);

	/// <summary>
	/// Creates a shader module from a file.
	/// </summary>
	/// <param name="logDevice">The logical device.</param>
	/// <param name="stage">Shader stage</param>
	/// <returns>Created shader module.</returns>
	static VkShaderModule createShaderModule(VkDevice logDevice, VkShaderStageFlagBits stage);

	static std::vector<VkShaderModule> createdModules;
}
