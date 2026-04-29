#version330 core

layout (points) in;
layout (points) out;
layout (max_vertoces = 30) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];

out float type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;

uniform float gDeltaTimeMillis;
uniform float gTime;
uniform sampler1D gRandomTexture;
uniform float gLaucherLifetime;
uniform float gshellLifetime;
uniform float gSecondaryShellLifetime;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f


vec3 GetrandomDir(float TexCoord)
{
	vec3 Dir = texture(gRandomTexture, TexCoord).xyz;
	Dir -= vec3(0.5, 0.5, 0.5);
	return Dir;
}

void main()
{
	float Age = Age0[0] + gDeltaTimeMillis;

	if (Type0[0] == PARTICLE_TYPE_LAUNCHER)
	{
		if (Age >= gLaucherLifetime)
		{
			Type1 = PARTICLE_TYPE_SHELL;
			Position1 = Position0[0];
			vec3 Dir = GetrandomDir(gTime / 1000.0);
			Dir.y = max(Dir.y, 0.5);
			Velocity1 = normalizw(Dir) / 20.0;
			Age1 = 0.0;
			EmitVertex();
			EndPrimitive();
			Age = 0.0;
		}
		type1 = PARTICLE_TYPE_LAUNCHER;
		Position1 = Position0[0]'
		Velocity1 = velocity0[0];
		age1 = age;
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		if (Age < gSecondaryShellLifetime)
		{
			Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
			Position1 = Position0[0] + DeltaP;
			Velocity1 = Velocity0[0] + DeltaV;
			Age1 = Age;
			EmitVertex();
			EndPrimitive();
		}
	}
}