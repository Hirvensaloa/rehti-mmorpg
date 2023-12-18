#ifdef CPP_INCLUDE_SHADERS
std::string characterVertexShaderSource = R"(
#version 450
// in
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in uvec4 boneIndices;
layout(location = 3) in vec4 boneWeights;
layout(location = 4) in vec2 texCoords;
// out
layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
// uniforms
layout(push_constant) uniform SingleMatrix
{ 
	mat4 viewmat;
} cameraData;

layout(set = 1, binding = 0) uniform SingleMatrix2
{
	mat4 modelmat;
} modelData;

layout(set = 1, binding = 1) uniform BoneUniform
{
	mat4 boneMatrices[50];
} boneData;

void main() {

	mat4 boneMatrix = boneData.boneMatrices[boneIndices[0]] * boneWeights[0];
	boneMatrix += boneData.boneMatrices[boneIndices[1]] * boneWeights[1];
	boneMatrix += boneData.boneMatrices[boneIndices[2]] * boneWeights[2];
	boneMatrix += boneData.boneMatrices[boneIndices[3]] * boneWeights[3];
	vec4 modelvertex = vec4(position, 1.f);
	// Combine both animation matrix and model matrix
	mat4 localToWorld = modelData.modelmat * boneMatrix;
	vec4 worldSpaceFrag = localToWorld * modelvertex;
    gl_Position = cameraData.viewmat * localToWorld * modelvertex;
	fragPos = worldSpaceFrag.xyz;
	fragNormal = vec3(transpose(inverse(localToWorld)) * vec4(normal, 0.f) );
	fragTexCoord = texCoords;
}

)";
#endif