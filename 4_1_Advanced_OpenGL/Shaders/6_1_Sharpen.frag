#version 330 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;

const float offset = 1.0/300.0;

void main()
{
	
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
	);

	float sharpen[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	vec3 applyTex[9];

	for(int i=0; i<9; ++i)
	{
		applyTex[i] = vec3(texture(screenTexture, texCoord.st + offsets[i]));
 	}

	vec3 color = vec3(0.0);

	for(int i=0; i<9; ++i)
	{
		color += applyTex[i] * sharpen[i];
	}
	
	fragColor = vec4(color, 1.0);
}