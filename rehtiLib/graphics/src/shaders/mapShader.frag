#ifdef CPP_INCLUDE_SHADERS
std::string mapFragmentShaderSource = R"(
#version 450
layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform DirectionalLight
{
	vec3 dir;
	vec3 color;
	float intensity;
} sun;

layout(set = 1, binding = 0) uniform sampler2D tex[6]; // blend, black, r, g, b, a

layout(location = 0) out vec4 outColor;

void main(){
	vec3 normal = normalize(fragNormal);
	float light = dot(normal, sun.dir);
	vec3 lightCol = sun.color * sun.intensity * light;
  vec4 blendCol = texture(tex[0], fragTexCoord);
  vec2 tiled = 20 * fragTexCoord; // TODO: change to a descriptor
  float black = 1 - (blendCol.r + blendCol.g + blendCol.b + blendCol.a);
  vec4 blackCol = texture(tex[1], tiled) * black;
  vec4 rCol = texture(tex[2], tiled) * blendCol.r;
  vec4 gCol = texture(tex[3], tiled) * blendCol.g;
  vec4 bCol = texture(tex[4], tiled) * blendCol.b;
  vec4 aCol = texture(tex[5], tiled) * blendCol.a;
 outColor = (blackCol + rCol + gCol + bCol + aCol);
}

)";
#endif