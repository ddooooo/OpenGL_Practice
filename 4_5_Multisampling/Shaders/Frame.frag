#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frameTexture;

void main()
{
    vec3 color = texture(frameTexture, TexCoords).rgb;
    float grayScale = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    FragColor = vec4(vec3(grayScale), 1.0);
}