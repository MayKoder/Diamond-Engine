#pragma once

#include "ParticleEffects.h";
class C_Transform;

class PE_Spawn_Area : public ParticleEffect //DEPRECATED CLASS TODO DELETE
{
public:
	PE_Spawn_Area();
	~PE_Spawn_Area() override;

	void Spawn(Particle& particle) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE
private:
	//Sphere with a point and a radius the point is an offset from the transform
	float centerOfSphere[3]; //(x,y,z)
	float radius;

};