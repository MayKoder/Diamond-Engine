#pragma once
#include "Particle.h"
#include <vector>

class Emitter
{
public:
	Emitter();
	~Emitter();

	void Update(float dt);
	void Draw();

#ifndef STANDALONE
	void OnEditor();
#endif // !STANDALONE

private:
	void CreateParticles();//TODO does this recieve smth?
	void ThrowParticles(float dt);
private:
	unsigned int maxParticles;
	float3 position;
	float particlesRate;
	std::vector<Particle> myParticles;
};