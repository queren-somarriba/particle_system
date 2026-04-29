#version 430 core

out vec4 FragColor;
in vec3 Position;

void main()
{
	FragColor = vec4(Position, 1.0);
}