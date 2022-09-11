#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

uniform vec3 cameraPos;
uniform samplerCube skybox;

vec3 reflection(vec3 N, vec3 V);

void main()
{
	vec3 V = normalize(cameraPos - Position);
	
	//vec3 R = reflect(-V, normalize(Normal));
	vec3 R = reflection(normalize(Normal), V);
	vec3 tex = texture(skybox, R).rgb;
	FragColor = vec4(tex.r, tex.g, tex.b, 1.0);
}

vec3 reflection(vec3 N, vec3 V)
{
	vec3 X = N*dot(N,V);
	return (2*X-V);
}