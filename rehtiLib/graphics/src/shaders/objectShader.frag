#ifdef CPP_INCLUDE_SHADERS
std::string objectFragmentShaderSource = R"(
#version 450
// in
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
// out
layout(location = 0) out vec4 fragColor;
// uniforms
layout(set = 0, binding = 0) uniform DirectionalLight
{
	vec3 dir;
	vec3 color;
	float intensity;
} sun;

layout (set = 0, binding = 1) uniform CameraData
{
	vec3 position;
	vec3 direction;
} camera;

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(set = 1, binding = 2) uniform Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

void main() {
	// setup
	vec3 normal = normalize(fragNormal);
	vec4 texColor = texture(texSampler, fragTexCoord);
	vec3 viewDir = normalize(camera.position - fragPos);
	vec3 reflectDir = reflect(sun.dir, normal); // sun dir is already pointing to the fragment
	float viewSimilarity = max(dot(viewDir, reflectDir), 0.f); // how close is the view to the reflection
	float lightAmount = max(dot(normal, -sun.dir), 0.f); // how much light is hitting the fragment
	float specularStrength = pow(viewSimilarity, material.shininess);
	// Components
	vec3 diffuse = lightAmount * material.diffuse * sun.color;
	vec3 ambient = material.ambient * sun.color;
	vec3 specular = specularStrength * material.specular * sun.color;
	vec4 combinedLight = vec4((ambient + diffuse + specular), 1.f) * sun.intensity;
	fragColor = combinedLight * texColor;
}

)";
#endif