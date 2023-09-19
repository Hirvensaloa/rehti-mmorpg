#ifdef CPP_INCLUDE_SHADERS
std::string fragmentShaderSource = R"(
#endif

#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}

#ifdef CPP_INCLUDE_SHADERS
)";
#endif