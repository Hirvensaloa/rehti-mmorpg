#ifdef CPP_INCLUDE_SHADERS
std::string mapFragmentShaderSource = R"(
#version 450
layout (location = 0) in vec2 texCoord;

layout(binding = 0) uniform sampler2D tex[6];

layout(location = 0) out vec4 outColor;

void main(){
 outColor = texture(tex[0], texCoord);
}

)";
#endif