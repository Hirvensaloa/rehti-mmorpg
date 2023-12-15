#ifdef CPP_INCLUDE_SHADERS
std::string objectVertexShaderSource = R"(
#version 450
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(push_constant) uniform SingleMatrix
{ 
	mat4 viewmat;
} cameraData;

layout(set = 0, binding = 0) uniform SingleMatrix2
{
	mat4 modelmat;
} modelData;

void main() {
    gl_Position = cameraData.viewmat * (modelData.modelmat * vec4(pos,  1.0));
    fragTexCoord = texCoord;
}

)";
#endif