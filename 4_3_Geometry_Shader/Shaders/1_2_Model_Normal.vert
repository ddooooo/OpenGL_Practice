#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
	vec3 normal;
} vs_out;



void main()
{
	vs_out.normal =  aNormal;

	gl_Position = vec4(aPos, 1.0);
}