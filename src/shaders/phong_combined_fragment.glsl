#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	uint shininess;
};

in vec3 DirectionalLightDir;
struct DirectionalLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 PointLightPos;
struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

in vec3 SpotLightPos;
in vec3 SpotLightDir;
struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutoffAngle;
	float outerCutoffAngle;
	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

vec3 calcDirectionalLight(DirectionalLight, vec3, vec3);
vec3 calcPointLight(PointLight, vec3, vec3);
vec3 calcSpotLight(SpotLight, vec3, vec3, vec3);

vec3 calcAmbientComponent(vec3);
vec3 calcDiffuseComponent(vec3, vec3, vec3);
vec3 calcSpecularComponent(vec3, vec3, vec3);
vec3 calcEmissionComponent();

void main()
{
	vec3 norm = normalize(Normal);
	vec3 directional = calcDirectionalLight(directionalLight, DirectionalLightDir, norm);
	vec3 point = calcPointLight(pointLight, PointLightPos, norm);
	vec3 spot = calcSpotLight(spotLight, SpotLightPos, SpotLightDir, norm);
	vec3 emission = calcEmissionComponent();

	FragColor = vec4(directional + point + spot, 1.0);
}

vec3 calcDiffuseComponent(vec3 diffuse, vec3 lightDirection, vec3 norm)
{
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = (vec3(texture(material.diffuse, TexCoords)) * diff) * diffuse;

	return diffuseColor;
}

vec3 calcSpecularComponent(vec3 specular, vec3 lightDirection, vec3 norm)
{
	vec3 viewDirection = normalize(-FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularIntensity = texture(material.specular, TexCoords).rgb;
	vec3 specularColor = (specularIntensity * spec) * specular;

	return specularColor;
}

vec3 calcAmbientComponent(vec3 ambient)
{
	return vec3(texture(material.diffuse, TexCoords)) * ambient;
}

vec3 calcEmissionComponent()
{
	vec3 specularIntensity = texture(material.specular, TexCoords).rgb;
	vec3 showEmission = step(vec3(1.0), vec3(1.0) - specularIntensity);
	vec3 emissionColor = texture(material.emission, TexCoords).rgb * showEmission;

	return emissionColor;
}

vec3 calcPointLight(PointLight light, vec3 lightPos, vec3 norm)
{
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 lightDirection = normalize(lightPos - FragPos);

	vec3 ambientColor = calcAmbientComponent(light.ambient);
	vec3 diffuseColor = calcDiffuseComponent(light.diffuse, lightDirection, norm);
	vec3 specularColor = calcSpecularComponent(light.specular, lightDirection, norm);

	ambientColor *= attenuation;
	diffuseColor *= attenuation;
	specularColor *= attenuation;

	return ambientColor + diffuseColor + specularColor;
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 lightDir, vec3 norm)
{
	vec3 lightDirection = normalize(lightDir);

	vec3 ambientColor = calcAmbientComponent(light.ambient);
	vec3 diffuseColor = calcDiffuseComponent(light.diffuse, lightDirection, norm);
	vec3 specularColor = calcSpecularComponent(light.specular, lightDirection, norm);

	return ambientColor + diffuseColor + specularColor;
}

vec3 calcSpotLight(SpotLight light, vec3 lightPos, vec3 lightDir, vec3 norm)
{
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 lightDirection = normalize(lightPos - FragPos);

	float theta = dot(lightDirection, normalize(lightDir));
	float epsilon = light.cutoffAngle - light.outerCutoffAngle;
	float intensity = clamp((theta - light.outerCutoffAngle) / epsilon, 0.0, 1.0);

	vec3 ambientColor = calcAmbientComponent(light.ambient);
	vec3 diffuseColor = calcDiffuseComponent(light.diffuse, lightDirection, norm);
	vec3 specularColor = calcSpecularComponent(light.specular, lightDirection, norm);

	ambientColor *= attenuation;
	diffuseColor = (diffuseColor * intensity) * attenuation;
	specularColor = (specularColor * intensity) * attenuation;

	return ambientColor + diffuseColor + specularColor;
}
