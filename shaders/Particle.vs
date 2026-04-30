#version 430 core

layout (location = 0) in vec3 p_position;
layout (location = 1) in vec3 p_color;

out vec3 Position;
out vec3 Color;

void main()
{
	Position = p_position;
	Color = p_color;
	gl_Position = vec4(p_position.x, p_position.y, p_position.z, 1.0);
}