#version 330 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height+16.0f)/32.0f; //(Height)*(64.0f/256.0f) - 16.0f;
    FragColor = vec4(h, h, h, 1.0f);
}