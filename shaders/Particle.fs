#version 430 core

out vec4 FragColor;
in vec3 Position;
in vec3 Color;
in vec3 Mpos;

void main()
{
	if (Position == Mpos)
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	FragColor = vec4(Color, 1.0);
}