#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in mat4 a_Model;

out vec4 Color;

uniform mat4 u_ProjectionView;

void main()
{
    gl_Position = u_ProjectionView * a_Model * vec4(a_Position, 1.0);
    Color = vec4(a_Color, 1.0);
}