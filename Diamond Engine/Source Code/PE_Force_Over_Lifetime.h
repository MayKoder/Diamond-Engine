#pragma once

#include "ParticleEffects.h"

class PE_ForceOverLifetime : public ParticleEffect
{
public:
	PE_ForceOverLifetime();
	~PE_ForceOverLifetime() override;

	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

public:
	float acceleration[3];
	float gravityModifier;
	float myGravity;
};
