#version 330 core
out vec4 FragColor;

in VS_OUT 
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

uniform samplerCube shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;

vec3 offsets[20] = vec3[]
(
	vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
	vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
	vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
	vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float CalcShadowPointLight(vec3 fragPos)
{
	vec3 frag_to_Light = fragPos - lightPos;
	float crnt_depth = length(frag_to_Light);

	float shadow = 0.0;
	float bias = 0.15;
	int n_sample = 20;
	float view_dist = length(viewPos - fragPos);
	float radius = (1.0 + (view_dist / far_plane)) / 25.0;

	// PCF for soft shadowing
	for(int i=0; i<n_sample; ++i)
	{
		float sampled_depth = texture(shadowMap, frag_to_Light + offsets[i]*radius).r;
		// Since the shadow map contain vertices in NDC space [0, 1]
		// depth needs to be multiplied by the far_plane to transform the depth value to eye(view) space
		sampled_depth *= far_plane; 
		if( (crnt_depth - sampled_depth) > bias )
			shadow += 1.0;
	}
	shadow /= float(n_sample);

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
	sf = pow(max(dot(normal, h) , 0.0), 32.0);
	vec3 specular = sf * lightColor;

	float shadow = CalcShadowPointLight(fs_in.fragPos);
	
	vec3 color = (ambient + (1.0 - shadow)) * (diffuse + specular); 

    FragColor = vec4(color, 1.0);
}