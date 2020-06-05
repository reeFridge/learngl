#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform uint shiness;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;

void main()
{
	vec3 ambientColor = ambientStrength * lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = diff * lightColor;
	vec3 viewDirection = normalize(-FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shiness);
	vec3 specularLight = specularStrength * specular * lightColor;
	vec3 resultColor = (ambientColor + diffuseColor + specularLight) * objColor;
    FragColor = vec4(resultColor, 1.0);
}
