#pragma once

#include "PE__Spawn_Shape_Base.h"

class PE_SpawnShapeSphere : public PE_SpawnShapeBase
{
public:
	PE_SpawnShapeSphere();
	~PE_SpawnShapeSphere() override;

	void Spawn(Particle& particle, bool hasInitialSpeed, float speed, float4x4& gTrans, float* offset) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	//Sphere with a point and a radius the point is an offset from the transform
	float radius;
};