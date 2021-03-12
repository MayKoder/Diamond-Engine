#pragma once

#include "ParticleEffects.h"

class PE_Rotate: public ParticleEffect
{
public:
	PE_Rotate();
	~PE_Rotate() override;

	void Spawn(Particle& particle) override;
	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif //!STANDALONE

private:
	float rotation;
	float rotationSpeed;
};