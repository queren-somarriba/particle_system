#version 430 core

layout (location = 0) in vec3 a_Position;
out vec3 Position;

void main()
{
	Position = a_Position;
	gl_Position = vec4(a_Position.x, a_Position.y, a_Position.z, 1.0);
}