#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <string>

// Shader sources
//#define CPP_INCLUDE_SHADERS
//
//#include "shader.vert"
//
//#include "shader.frag"

typedef char const * const ConstantFilepath;

namespace ShaderManager
{
	// Constants
	constexpr ConstantFilepath kVertexShaderPath = "C:/Users/otsol/source/repos/rehti-mmorpg/rehtiLib/graphics/src/shaders/vertexShader.spv";
	constexpr ConstantFilepath kFragmentShaderPath = "C:/Users/otsol/source/repos/rehti-mmorpg/rehtiLib/graphics/src/shaders/fragmentShader.spv";
	constexpr ConstantFilepath kGLSLVertexShaderPath = "shaders/shader.vert";
	constexpr ConstantFilepath kGLSLFragmentShaderPath = "shaders/shader.frag";

	/// <summary>
	/// Creates a vertex shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(VkDevice logDeviceP);

	/// <summary>
	/// Creates a fragment shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(VkDevice logDeviceP);

	/// <summary>
	/// Destroys created shader modules. This function can be called after pipeline creation.
	/// </summary>
	/// <param name="logDeviceP">The logical device.</param>
	void destroyShaderModules(VkDevice logDeviceP);

	/// <summary>
	/// Reads a file and returns a vector of chars.
	/// </summary>
	/// <param name="rFilenameP">Name of the file to read.</param>
	/// <returns>A vector of characters read from the file.</returns>
	static std::vector<char> readFile(ConstantFilepath filepathP);

	/// <summary>
	/// Creates a shader module from a file.
	/// </summary>
	/// <param name="logDeviceP">The logical device.</param>
	/// <param name="filepathP">Path to shader file.</param>
	/// <returns>Created shader module.</returns>
	static VkShaderModule createShaderModule(VkDevice logDeviceP, ConstantFilepath filepathP);

	static std::vector<VkShaderModule> createdModules;
}
