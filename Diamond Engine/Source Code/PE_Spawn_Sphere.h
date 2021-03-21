#pragma once

#include "ParticleEffects.h";
class C_Transform;

class PE_SpawnSphere : public ParticleEffect //DEPRECATED CLASS TODO DELETE
{
public:
	PE_SpawnSphere();
	~PE_SpawnSphere() override;

	void Spawn(Particle& particle) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	//Sphere with a point and a radius the point is an offset from the transform
	float centerOfSphere[3]; //(x,y,z)
	float radius;
};