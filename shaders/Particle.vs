#version 430 core

layout (location = 0) in vec3 p_position;
layout (location = 1) in vec3 p_color;

out vec3 Color;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	Color = p_color;
	gl_Position = proj * view * vec4(p_position, 1.0);
}