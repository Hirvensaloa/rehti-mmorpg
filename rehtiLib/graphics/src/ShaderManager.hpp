#pragma once
#include "GraphicsTypes.hpp"

#include <string>

typedef char const* const ConstantFilepath;

namespace ShaderManager
{
	// Constants
	constexpr ConstantFilepath kGLSLTestVertexShaderPath = "shaders/testShader.vert";
	constexpr ConstantFilepath kGLSLTestFragmentShaderPath = "shaders/testShader.frag";
	constexpr ConstantFilepath kGLSLObjectVertexShaderPath = "shaders/objectShader.vert";
	constexpr ConstantFilepath kGLSLObjectFragmentShaderPath = "shaders/objectShader.frag";
	constexpr const char* kStandardEntryPoint = "main";

	/// <summary>
	/// Creates a vertex shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(VkDevice logDevice, ObjectType objectType);

	/// <summary>
	/// Creates a fragment shader info to be used in pipeline creation.
	/// </summary>
	/// <param name="logDevice">Logical device.</param>
	/// <returns>Creation info.</returns>
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(VkDevice logDevice, ObjectType objectType);

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
	std::vector<uint32_t> compileGLSLToSpirv(VkShaderStageFlagBits shaderType, ObjectType object);

	/**
	 * @brief Returns the shader code as a string corresponding to the given stage and object type.
	 * @param stage of the shader
	 * @param object type
	 * @return shader code as a string
	*/
	std::string getShaderCode(VkShaderStageFlags stage, ObjectType object);

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
	static VkShaderModule createShaderModule(VkDevice logDevice, VkShaderStageFlagBits stage, ObjectType objectType);

	static std::vector<VkShaderModule> createdModules;
}
