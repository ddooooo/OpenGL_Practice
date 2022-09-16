#version 330 core
layout (location = 0) in vec3 in_Pos;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoords;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool reverse_normals;

void main()
{
	vs_out.fragPos = vec3(model*vec4(in_Pos, 1.0));
	
	if(reverse_normals)
		vs_out.normal = transpose(inverse(mat3(model))) * (-1*in_Normal);
	else
		vs_out.normal = transpose(inverse(mat3(model))) * in_Normal;
	
	vs_out.texCoords = in_TexCoords;

	gl_Position = projection * view * model * vec4(in_Pos, 1.0);
}