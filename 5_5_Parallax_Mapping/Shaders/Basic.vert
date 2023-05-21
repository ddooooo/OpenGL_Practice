#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out VS_OUT
{
	vec3 FragPos;
	vec3 TangentLight;
	vec3 TangentView;
	vec3 TangentFrag;
	vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 N = normalize(normalMatrix * aNormal);
	vec3 T = normalize(normalMatrix * aTangent);
	
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);


	mat3 inverseTBN = transpose(mat3(T, B, N));
	vs_out.TangentLight = inverseTBN * lightPos;
	vs_out.TangentView = inverseTBN * viewPos;
	vs_out.TangentFrag = inverseTBN * vs_out.FragPos;

	vs_out.TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}