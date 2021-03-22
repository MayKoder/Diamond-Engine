#pragma once

#include "ParticleEffects.h";
class C_Transform;

class PE_SpawnCone : public ParticleEffect //DEPRECATED CLASS TODO DELETE
{
public:
	PE_SpawnCone();
	~PE_SpawnCone() override;

	void Spawn(Particle& particle) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	float coneTip[3]; //(x,y,z)
	float centerOfSphere[3]; //(x,y,z)
	float radius;
};