#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	uint shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;

void main()
{
	vec3 ambientColor = material.ambient * light.ambient;
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = (material.diffuse * diff) * light.diffuse;
	vec3 viewDirection = normalize(-FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularLight = (material.specular * specular) * light.specular;
	vec3 resultColor = ambientColor + diffuseColor + specularLight;
    FragColor = vec4(resultColor, 1.0);
}
