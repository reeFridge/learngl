#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	uint shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 ResultColor;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightPos;
uniform Material material;
uniform Light light;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vec3 vertexPos = vec3(view * model * vec4(aPos, 1.0));
	vec3 norm = normalize(normalMatrix * aNormal);
	vec3 viewLightPos = vec3(view * vec4(lightPos, 1.0));

	//ambient
	vec3 ambientColor = vec3(texture(material.diffuse, aTexCoords)) * light.ambient;
	// diffuse
	vec3 lightDirection = normalize(viewLightPos - vertexPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = (vec3(texture(material.diffuse, aTexCoords)) * diff) * light.diffuse;
	// specular
	vec3 viewDirection = normalize(-vertexPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularLight = (vec3(texture(material.specular, aTexCoords)) * specular) * light.specular;
	vec3 emissionColor = texture(material.emission, aTexCoords).rgb * step(vec3(1.0), vec3(1.0) - texture(material.specular, aTexCoords).rgb);
	ResultColor = ambientColor + diffuseColor + specularLight + emissionColor;
}
