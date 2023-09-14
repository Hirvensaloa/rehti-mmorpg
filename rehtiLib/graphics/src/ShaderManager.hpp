#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <string>

typedef char const * const ConstantFilepath;
namespace ShaderManager
{
	// Constants
	constexpr ConstantFilepath kVertexShaderPath = "shaders/vert.spv";
	constexpr ConstantFilepath kFragmentShaderPath = "shaders/frag.spv";
	constexpr ConstantFilepath kGLSLVertexShaderPath = "shaders/shader.vert";
	constexpr ConstantFilepath kGLSLFragmentShaderPath = "shaders/shader.frag";


	VkShaderModule createShaderModule(VkDevice logDeviceP, ConstantFilepath filepathP);

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
	/// Reads a file and returns a vector of chars.
	/// </summary>
	/// <param name="rFilenameP">Name of the file to read.</param>
	/// <returns>A vector of characters read from the file.</returns>
	static std::vector<char> readFile(ConstantFilepath filepathP);

}
