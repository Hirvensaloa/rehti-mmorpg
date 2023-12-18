#ifdef CPP_INCLUDE_SHADERS
std::string testFragmentShaderSource = R"(
#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform DirectionalLight
{
	vec3 dir;
	vec3 color;
	float intensity;
} sun;

void main() {
	vec3 newColor = fragColor * sun.color * sun.intensity;
    outColor = vec4(newColor, 1.0);
}

)";
#endif