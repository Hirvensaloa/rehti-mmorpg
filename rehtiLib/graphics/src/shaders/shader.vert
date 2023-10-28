#ifdef CPP_INCLUDE_SHADERS
std::string vertexShaderSource = R"(
#version 450
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform SingleMatrix
{ 
	mat4 viewmat;
} cameraData;

layout(set = 0, binding = 0) uniform SingleMatrix2
{
	mat4 modelmat;
} modelData;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = cameraData.viewmat * (modelData.modelmat * vec4(pos,  1.0));
    fragColor = color;
}

)";
#endif