#version 330 core

in vec4 Color;
in vec2 Uv;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    FragColor = Color;
}