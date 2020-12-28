#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform vec3 pointLightPos;
out vec3 PointLightPos;

uniform vec3 directionalLightDir;
out vec3 DirectionalLightDir;

uniform vec3 spotLightPos;
out vec3 SpotLightPos;

uniform vec3 spotLightDir;
out vec3 SpotLightDir;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = normalMatrix * aNormal;
	TexCoords = aTexCoords;

	PointLightPos = vec3(view * vec4(pointLightPos, 1.0));
	DirectionalLightDir = normalMatrix * normalize(-directionalLightDir);
	SpotLightPos = vec3(view  * vec4(spotLightPos, 1.0));
	SpotLightDir = normalMatrix * normalize(-spotLightDir);
}
