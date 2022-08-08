#version 330 core

out vec4 fragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight 
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};

#define NR_POINT_LIGHTS 4 

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform Material material;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// Phong Lighting Model -> light Intensity * (specular + diffuse + ambient)
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// Directional Light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	

	// Point Light
	for(int i=0; i<4; ++i)
	{
		result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
	}
	
	// Spot Light
	result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

	fragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir =	normalize(-light.direction);

	// Ambient
	vec3  ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

	// Specular
	vec3 reflectDir = 2 * dot(normal, lightDir) * normal - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	vec3 specular = light.specular * specFactor * vec3(texture(material.specular, texCoords));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos); 

	// Ambient
	vec3  ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

	// Specular
	vec3 reflectDir = 2 * dot(normal, lightDir) * normal - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	vec3 specular = light.specular * specFactor * vec3(texture(material.specular, texCoords));

	// Distance Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return attenuation*(ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// Ambient
	vec3  ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

	// Specular
	vec3 reflectDir = 2 * dot(normal, lightDir) * normal - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	vec3 specular = light.specular * specFactor * vec3(texture(material.specular, texCoords));

	// Calculate spotlight
	float theta = dot(lightDir, -light.direction);
	float epsilon =  light.cutOff - light.outerCutOff;
	float intensity = (theta - light.outerCutOff)/epsilon;
	intensity = clamp(intensity, 0.0, 1.0);

	// Distance Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return attenuation * intensity * (ambient + diffuse + specular);
}
