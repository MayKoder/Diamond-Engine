#pragma once

#include "ParticleEffects.h";
class C_Transform;

class PE_SpawnArea : public ParticleEffect //DEPRECATED CLASS TODO DELETE
{
public:
	PE_SpawnArea();
	~PE_SpawnArea() override;

	void Spawn(Particle& particle) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	//Quad with a point and a radius the point is an offset from the transform
	float centerOfQuad[3]; //(x,y,z)
	float radius;
};