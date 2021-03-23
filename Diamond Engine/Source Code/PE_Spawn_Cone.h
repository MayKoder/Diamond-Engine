#pragma once

#include "PE__Spawn_Shape_Base.h"

class PE_SpawnShapeCone : public PE_SpawnShapeBase
{
public:
	PE_SpawnShapeCone();
	~PE_SpawnShapeCone() override;

	void Spawn(Particle& particle,bool hasInitialSpeed, float speed) override; //Spawns in area

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