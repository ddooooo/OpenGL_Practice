#version 330 core


struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
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

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;

void main()
{
	// Phong Lighting Model -> light Intensity * (specular + diffuse + ambient)

	// Caculate Ambient
	//float ambientStrength = 0.1;
	vec3  ambient = light.ambient * texture(material.diffuse, texCoords).rgb; //material.ambient;

	// Calculate Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diffFactor = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diffFactor * texture(material.diffuse, texCoords).rgb;

	// Calculate Specular
	//float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);	
	vec3 specular = light.specular * specFactor * texture(material.specular, texCoords).rgb;	

	// Calculate Emission
	vec3 emission = vec3(0.0);
	if(texture(material.specular, texCoords).rgb == vec3(0.0))
	{
		emission = texture(material.emission, texCoords).rgb;
	}

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 result = attenuation * (ambient + diffuse + specular + emission);
	fragColor = vec4(result, 1.0);
}