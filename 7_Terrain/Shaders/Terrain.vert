#version 330 core
layout (location = 0) in vec3 in_Pos;

out float Height;
out vec3 Pos;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    Height = in_Pos.y;
    float adjust_Pos_y = in_Pos.y * 10.0f; 
    Pos = (view * model * vec4(in_Pos, 1.0)).xyz;
	gl_Position = projection * view * model * vec4(in_Pos.x, adjust_Pos_y, in_Pos.z, 1.0);
}