#pragma once

#include "ParticleEffects.h"

class PE_VelocityOverLifetime : public ParticleEffect
{
public:
	PE_VelocityOverLifetime();
	~PE_VelocityOverLifetime() override;

	void Spawn(Particle& particle) override;
	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

public:
	float velocity[3];
};