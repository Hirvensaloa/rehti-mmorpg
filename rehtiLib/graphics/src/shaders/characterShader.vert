#ifdef CPP_INCLUDE_SHADERS
std::string characterVertexShaderSource = R"(
#version 450

void main() {
    gl_Position = vec4(1.f); // TODO
}

)";
#endif