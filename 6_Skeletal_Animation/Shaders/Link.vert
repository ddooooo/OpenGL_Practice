#version 330 core
layout (location = 0) in vec3 in_Pos;
layout (location = 1) in vec3 in_Normal;
//layout (location = 2) in vec3 in_Tangent;
layout (location = 3) in vec2 in_TexCoord;
layout (location = 4) in ivec4 in_BoneIDs;
layout (location = 5) in vec4 in_Weights;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;
flat out ivec4 boneIDs;
out vec4 weights;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = vec3(model * vec4(in_Pos, 1.0));
	normal = mat3(transpose(inverse(model))) * in_Normal;

	boneIDs = in_BoneIDs;
	weights = in_Weights;

	texCoord = in_TexCoord;

	gl_Position = projection * view * model * vec4(in_Pos, 1.0);
}