#ifndef __PE_MOVE_H__
#define __PE_MOVE_H__

#include "ParticleEffects.h"

class PE_Move : public ParticleEffect
{
public:
	PE_Move();
	~PE_Move() override;

	void Spawn(Particle& particle) override;
	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

public:
	float speed[3];
	float acceleration[3];
};

#endif // !__PE_MOVE_H__
