#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec3 TangentLight;
	vec3 TangentView;
	vec3 TangentFrag;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

uniform float height_scale;

vec2 ParallaxMap(vec2 A, vec3 V)
{
	float h_A = texture(heightMap, A).r;
	vec2 P = (V.xy/V.z) * (h_A * height_scale);
	return A - P;
}

vec2 SteepParallaxMap(vec2 texCoords, vec3 V)
{
	const float min_layers = 8.0;
	const float max_layers = 32.0;
	float num_layers = mix( max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), V)) );
	float layer_depth = 1.0 / num_layers;

	float crnt_layer_depth = 0.0;

	vec2 P = (V.xy/V.z) * height_scale;
	vec2 delta = P / num_layers;

	vec2 crnt_texCoords = texCoords;
	float crnt_depth_value = texture(heightMap, crnt_texCoords).r;

	while(crnt_layer_depth < crnt_depth_value)
	{
		crnt_texCoords -= delta;
		crnt_depth_value = texture(heightMap, crnt_texCoords).r;
		crnt_layer_depth += layer_depth;
	}

	return crnt_texCoords;
}

vec2 ParallaxOcculsionMap(vec2 texCoords, vec3 V)
{
	const float min_layers = 8.0;
	const float max_layers = 32.0;
	float num_layers = mix( max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), V)) );
	float layer = 1.0 / num_layers;

	float crnt_layer = 0.0;

	vec2 P = (V.xy/V.z) * height_scale;
	vec2 delta = P / num_layers;

	vec2 crnt_texCoords = texCoords;
	float crnt_depth = texture(heightMap, crnt_texCoords).r;

	while(crnt_layer < crnt_depth)
	{
		crnt_texCoords -= delta;
		crnt_depth = texture(heightMap, crnt_texCoords).r;
		crnt_layer += layer;
	}

	vec2 prev_texCoords = crnt_texCoords + delta;
	
	float after_depth = crnt_depth - crnt_layer;
	float before_depth = texture(heightMap, prev_texCoords).r - (crnt_layer - layer);

	float w = after_depth / (after_depth - before_depth);

	vec2 final_texCoords = w * prev_texCoords + (1-w) * crnt_texCoords;

	return final_texCoords;
}

bool OutOfRange(vec2 texCoords)
{
	if(texCoords.x > 1.0 || texCoords.x < 0.0)
	{
		return true;
	}
	else if(texCoords.y > 1.0 || texCoords.y < 0.0)
	{
		return true;
	}

	return false;
}

void main()
{
	vec3 V = normalize(fs_in.TangentView - fs_in.TangentFrag);
	vec2 texCoords = fs_in.TexCoords;

	texCoords = ParallaxOcculsionMap(texCoords, V); //SteepParallaxMap(texCoords, V); //ParallaxMap(texCoords, V);

	if(OutOfRange(texCoords))
	{
		discard;
	}

	vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal*2.0 - 1.0);
	
	vec3 color = texture(diffuseMap, texCoords).rgb;

	vec3 ambient = 0.1 * color;

	vec3 lightDir = normalize(fs_in.TangentLight - fs_in.TangentFrag);
	float df = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = df * color;

	vec3 halfwayDir = normalize(lightDir + V);
	float sf = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

	vec3 specular = vec3(0.2) * sf;

	FragColor = vec4(ambient + diffuse + specular, 1.0f);

}