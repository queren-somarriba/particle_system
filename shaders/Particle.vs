#version 430 core

layout (location = 0) in vec3 p_position;
layout (location = 1) in vec3 p_color;

out vec3 Position;
out vec3 Color;
out vec3 Mpos;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 mouse;

void main()
{
	Mpos = vec3(mouse);
	Position = p_position;
	Color = p_color;
	gl_Position = projection * view * vec4(p_position, 1.0);
}