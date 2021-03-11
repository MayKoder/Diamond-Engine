#ifndef __PE_SPAWN_H__
#define __PE_SPAWN_H__

#include "ParticleEffects.h"

class PE_Spawn : public ParticleEffect
{
public:
	PE_Spawn();
	~PE_Spawn() override;

	void Spawn(Particle& particle) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

private:
	float spawnPos[3];
};


#endif // !__PE_SPAWN_H__
