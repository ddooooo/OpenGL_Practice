#version 330 core

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	// Phong Lighting Model -> light Intensity * (Specular + diffuse + ambient)

	// Caculate Ambient
	float ambientStrength = 0.1;
	vec3  ambient = ambientStrength * lightColor;

	// Calculate Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffFactor = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffFactor * lightColor;

	// Calculate Specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir;
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);	
	vec3 specular = specularStrength * specFactor * lightColor;	

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, 1.0);
}