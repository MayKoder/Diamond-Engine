#pragma once
#include "Particle.h"
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
	unsigned int maxParticles;
	float3 position;
	float particlesRate;
	std::vector<Particle> myParticles;
};