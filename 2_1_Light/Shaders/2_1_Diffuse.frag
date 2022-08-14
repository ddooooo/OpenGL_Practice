#version 330 core

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	float ambientStrength = 0.1;
	vec3  ambient = ambientStrength * lightColor;

	// Calculate Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffFactor = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffFactor * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;
	fragColor = vec4(result, 1.0);
}