#include "ParticleEmitter.h"

Emitter::Emitter():maxParticles(0),position(0.0f,0.0f,0.0f),particlesRate(0)//TODO do we need to pass values to the constructor?
{

}

Emitter::~Emitter()
{
}

void Emitter::Update(float dt)
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

void Emitter::CreateParticles()
{
}

void Emitter::ThrowParticles(float dt)
{
}
