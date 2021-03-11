#include "ParticleEmitter.h"

Emitter::Emitter():
	maxParticles(0),
	position(0.0f,0.0f,0.0f),
	particlesRate(0)
{

}

Emitter::~Emitter()
{
}

void Emitter::Update(float dt,	bool systemActive)
{
}

void Emitter::Draw()
{
}

#ifndef STANDALONE
void Emitter::OnEditor()
{
}
#endif // !STANDALONE

void Emitter::SetPoolSize(unsigned int poolSize)
{
	myParticles.resize(poolSize);
}

void Emitter::CreateParticles(unsigned int particlesToAdd)
{
	myParticles.resize(myParticles.size() + particlesToAdd);
}

void Emitter::ThrowParticles(float dt)
{
}
