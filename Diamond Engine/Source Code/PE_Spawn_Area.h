#pragma once

#include "ParticleEffects.h";

class PE_Spawn_Area : ParticleEffect
{
public:
	PE_Spawn_Area();
	~PE_Spawn_Area() override;

	void Spawn(Particle& particle) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE
private:
	//Sphere with a point and a radius
	float centerOfSphere[3]; //(x,y,z)
	float radius;
};