#version 330 core
out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
	if(texture(skybox, texCoord).rgb == vec3(0.0))
	{
		fragColor == vec4(1.0, 1.0, 1.0, 0.0);
	}
	else
	{
		fragColor = texture(skybox, texCoord);
	}
}