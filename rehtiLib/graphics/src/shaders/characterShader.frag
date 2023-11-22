#ifdef CPP_INCLUDE_SHADERS
std::string characterFragmentShaderSource = R"(
#version 450
layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 fragColor;

layout(binding = 2) uniform sampler2D tex;

void main(){
	fragColor = texture(tex, texCoord);
}

)";
#endif