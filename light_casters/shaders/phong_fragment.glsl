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
	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;

void main()
{
	float distance = length(LightPos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	vec3 ambientColor = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = (vec3(texture(material.diffuse, TexCoords)) * diff) * light.diffuse;
	vec3 viewDirection = normalize(-FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularIntensity = texture(material.specular, TexCoords).rgb;
	vec3 specularLight = (specularIntensity * specular) * light.specular;
	vec3 showEmission = step(vec3(1.0), vec3(1.0) - specularIntensity);
	vec3 emissionColor = texture(material.emission, TexCoords).rgb * showEmission;
	ambientColor *= attenuation;
	diffuseColor *= attenuation;
	specularLight *= attenuation;
	emissionColor *= attenuation;
	vec3 resultColor = ambientColor + diffuseColor + specularLight + emissionColor;
    FragColor = vec4(resultColor, 1.0);
}
