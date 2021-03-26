#pragma once

#include "ParticleEffects.h"


enum class FORCE_TYPE
{
	DIRECTION,
	POINT,
	MAX
};

class PE_ForceOverLifetime : public ParticleEffect
{
public:
	PE_ForceOverLifetime();
	~PE_ForceOverLifetime() override;

	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	std::string ForceTypeToString(FORCE_TYPE type);

public:
	FORCE_TYPE forceType;
	float direction[3];
	float forceMagitude;
	float gravityModifier;
	float myGravity;
};
