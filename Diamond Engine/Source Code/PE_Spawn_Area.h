#pragma once

#include "PE__Spawn_Shape_Base.h"

class PE_SpawnShapeArea : public PE_SpawnShapeBase
{
public:
	PE_SpawnShapeArea();
	~PE_SpawnShapeArea() override;

	void Spawn(Particle& particle, bool hasInitialSpeed,float speed, float4x4& gTrans, float* offset) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	float dimensions[3];
};