#ifdef CPP_INCLUDE_SHADERS
std::string mapFragmentShaderSource = R"(
#version 450
layout (location = 0) in vec2 texCoord;

layout(binding = 0) uniform sampler2D tex[6]; // blend, black, r,g,b,a

layout(location = 0) out vec4 outColor;

void main(){
vec4 blendCol = texture(tex[0], texCoord); 
vec2 tiled = 20 * texCoord; // TODO change to a descriptor
float black = 1 - (blendCol.r + blendCol.g + blendCol.b + blendCol.a);
vec4 blackCol = texture(tex[1], tiled) * black;
vec4 rCol = texture(tex[2], tiled) * blendCol.r;
vec4 gCol = texture(tex[3], tiled) * blendCol.g;
vec4 bCol = texture(tex[4], tiled) * blendCol.b;
vec4 aCol = texture(tex[5], tiled) * blendCol.a;
 outColor = blackCol + rCol + gCol + bCol + aCol;
}

)";
#endif