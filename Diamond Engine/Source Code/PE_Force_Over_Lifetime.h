#ifndef __PE_MOVE_H__
#define __PE_MOVE_H__

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

#endif // !__PE_MOVE_H__
