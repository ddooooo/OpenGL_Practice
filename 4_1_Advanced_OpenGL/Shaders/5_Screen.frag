#version 330 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;

void main()
{
	vec3 color = vec3(texture(screenTexture, texCoord));
	fragColor = vec4(color, 1.0);
}