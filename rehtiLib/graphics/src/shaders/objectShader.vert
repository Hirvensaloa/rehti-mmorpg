#ifdef CPP_INCLUDE_SHADERS
std::string objectVertexShaderSource = R"(
#version 450
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

layout(push_constant) uniform SingleMatrix
{ 
	mat4 viewmat;
} cameraData;

layout(set = 1, binding = 0) uniform SingleMatrix2
{
	mat4 modelmat;
} modelData;

void main() {
	vec4 worldSpaceFrag = modelData.modelmat * vec4(pos,  1.0);
	fragPos = worldSpaceFrag.xyz;
	fragTexCoord = texCoord;
	fragNormal = transpose(inverse(mat3(modelData.modelmat))) * normal;
    gl_Position = cameraData.viewmat * worldSpaceFrag;
}

)";
#endif