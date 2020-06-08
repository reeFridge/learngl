#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 ResultColor;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightPos;
uniform uint shiness;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vec3 vertexPos = vec3(view * model * vec4(aPos, 1.0));
	vec3 norm = normalize(normalMatrix * aNormal);
	vec3 viewLightPos = vec3(view * vec4(lightPos, 1.0));

	//ambient
	vec3 ambientColor = ambientStrength * lightColor;
	// diffuse
	vec3 lightDirection = normalize(viewLightPos - vertexPos);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = diff * lightColor;
	// specular
	vec3 viewDirection = normalize(-vertexPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shiness);
	vec3 specularLight = specularStrength * specular * lightColor;
	ResultColor = ambientColor + diffuseColor + specularLight;
}
