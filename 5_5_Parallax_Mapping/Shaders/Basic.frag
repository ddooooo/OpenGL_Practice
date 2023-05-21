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

uniform sampler2D floor;
uniform sampler2D normalMap;
uniform bool blinn;

void main()
{
	vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
	normal = normalize(normal*2.0 - 1.0);

	vec3 color = texture(floor, fs_in.TexCoords).rgb;

	vec3 ambient = 0.5 * color;

	vec3 lightDir = normalize(fs_in.TangentLight - fs_in.TangentFrag);
	float df = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = df * color;

	vec3 v = normalize(fs_in.TangentView - fs_in.TangentFrag);
	vec3 r = reflect(-lightDir, normal);

	float sf = 0.0;
	if(blinn)
	{
		vec3 halfwayDir = normalize(lightDir + v);
		sf = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	}
	else
	{
		sf = pow(max(dot(v, r), 0.0), 8.0);
	}

	vec3 specular = vec3(0.5) * sf;

	FragColor = vec4(0.0, 0.0, 1.0, 1.0);  // vec4(ambient + diffuse + specular, 1.0f);

}