#pragma once
#include "Particle.h"
#include "ParticleEffects.h"
#include <vector>

class Emitter
{
public:
	Emitter();
	~Emitter();

	void Update(float dt, bool systemActive);
	void Draw();

#ifndef STANDALONE
	void OnEditor();
#endif // !STANDALONE

private:
	void SetPoolSize(unsigned int poolSize);
	//adds particles to the pool
	void CreateParticles(unsigned int particlesToAdd);
	void ThrowParticles(float dt);

private:
	float3 position;
	//particles per second
	int particlesPerSec;
	//this variable holds the extra time from the particle spawn last frame
	float lastParticeTime;
	std::vector<Particle> myParticles;
	std::vector<ParticleEffect> myEffects;
};