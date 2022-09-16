#version 330 core
out vec4 FragColor;

in VS_OUT 
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 lightPos;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 lightPos, float cos_theta)
{
	vec3 projCoords = lightPos.xyz / lightPos.w; // Make homogenous coordinate to 1 for perspective divide
	projCoords = projCoords * 0.5 + 0.5; // Convert [-1, 1] => [0, 1]

	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	// If current depth is larger than closest depth
	// then it is in the shadow
	// The finite precision of floating point can cause the shadow acne
	// Need to add small float point to the closest depth
	float bias = mix(0.01, 0.005, cos_theta);
	float shadow = 0.0;
	vec2 offset = 1.0/textureSize(shadowMap, 0);
	for(int i=-1; i<=1; ++i)
	{
		for(int j=-1; j<=1; ++j)
		{
			float depth = texture(shadowMap, projCoords.xy + vec2(i,j)*offset).r;
			shadow += (currentDepth - depth) > bias ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;
}

void main()
{
	vec3 normal = normalize(fs_in.normal);
	vec3 lightColor = vec3(1.0);

	vec3 ambient = 0.3 * lightColor;

	vec3 lightDir = normalize(lightPos - fs_in.fragPos);
	float cos_theta = dot(lightDir, normal);
	float df = max(cos_theta, 0.0); // Diffuse factor
	vec3 diffuse = df * lightColor;

	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	vec3 r = reflect(-lightDir, normal); // Reflect direction
	float sf = 0.0f; // Specular factor
	vec3 h = normalize(lightDir + viewDir); // Blinn-Phong specular calculation
	sf = pow(max(dot(normal, h) , 0.0), 64.0);
	vec3 specular = sf * lightColor;

	float shadow = ShadowCalculation(fs_in.lightPos, cos_theta);
	
	vec3 color = (ambient + (1.0 - shadow)) * (diffuse + specular); 

    FragColor = vec4(color, 1.0);
}