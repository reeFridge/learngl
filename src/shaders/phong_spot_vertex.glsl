#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;
out vec2 TexCoords;
out vec3 SpotDir;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 lightPos;
uniform vec3 spotDir;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
	// interpolates into specific fragment position in fragment shader
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = normalMatrix * aNormal;
	LightPos = vec3(view  * vec4(lightPos, 1.0));
	SpotDir = normalMatrix * normalize(-spotDir);
	TexCoords = aTexCoords;
}
