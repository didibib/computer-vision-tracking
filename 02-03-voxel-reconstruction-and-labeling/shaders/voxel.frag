#version 330 core

in vec4 Color;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    FragColor = Color;
}