#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 lightPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightMatrix;

void main()
{
	vs_out.fragPos = vec3(model*vec4(inPos, 1.0));
	vs_out.normal = transpose(inverse(mat3(model))) * inNormal;
	vs_out.texCoords = inTexCoords;
	vs_out.lightPos = lightMatrix * vec4(vs_out.fragPos, 1.0);

	gl_Position = projection * view * model * vec4(inPos, 1.0);
}