#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_Uv;
layout (location = 3) in vec3 a_Normal;

out vec4 Color;
out vec2 Uv;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_NormalMatrix; 

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    gl_PointSize = 1000;
    Color = vec4(a_Color, 1.0);
    Uv = a_Uv;
    Normal = mat3(u_NormalMatrix) * a_Normal;
    FragPos = vec3(u_Model * vec4(a_Position, 1.0));
}