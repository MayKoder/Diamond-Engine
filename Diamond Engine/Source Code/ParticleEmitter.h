#pragma once
#include <vector>
#include "Particle.h"
#include "ParticleEffects.h"


class Emitter
{
public:
	Emitter();
	~Emitter();

	void Update(float dt, bool systemActive);
	void Draw();

#ifndef STANDALONE
	void OnEditor(int emitterIndex);
#endif // !STANDALONE

private:
	//calculates a new Pool Size from the particle spawn rate & particle lifeTime
	void CalculatePoolSize();
	void SetPoolSize(unsigned int poolSize);
	//adds particles to the pool
	void CreateParticles(unsigned int particlesToAdd);
	void ThrowParticles(float dt);
	//returns index if it exists, else returns -1
	int DoesEffectExist(PARTICLE_EFFECT_TYPE type);

	std::string ParticleEffectEnumToString(PARTICLE_EFFECT_TYPE type);
	ParticleEffect* CreateEffect(PARTICLE_EFFECT_TYPE type);

public:
	bool toDelete;
private:

	//Min - Max particles lifeTime 
	float particlesLifeTime[2];

	//particles per second
	int particlesPerSec;
	//this variable holds the extra time from the particle spawn last frame
	float lastParticeTime;
	std::vector<Particle> myParticles;
	std::vector<ParticleEffect*> myEffects;
};