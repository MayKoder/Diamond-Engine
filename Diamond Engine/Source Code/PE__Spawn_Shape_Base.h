#pragma once
#include"DEJsonSupport.h"

struct Particle;

enum class PE_SPAWN_SHAPE_TYPE : int
{
	AREA,
	SPHERE,
	CONE,
	MAX
};

class PE_SpawnShapeBase
{
public:
	PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE type);
	virtual ~PE_SpawnShapeBase();

	virtual void Spawn(Particle& particle,bool hasInitialSpeed,float speed,float4x4& gTrans,float* offset);

#ifndef STANDALONE
	virtual void OnEditor(int emitterIndex);
#endif // !STANDALONE

	virtual void SaveData(JSON_Object* nObj);
	virtual void LoadData(DEConfig& nObj);

	PE_SPAWN_SHAPE_TYPE GetType()const;
protected:
	
	PE_SPAWN_SHAPE_TYPE myType;
};
