#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

void main()
{
	vec3 result = vec3(texture(texture_diffuse1, texCoords));

	fragColor = vec4(result, 1.0);
}