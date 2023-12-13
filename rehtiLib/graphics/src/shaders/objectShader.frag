#ifdef CPP_INCLUDE_SHADERS
std::string objectFragmentShaderSource = R"(
#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}

)";
#endif