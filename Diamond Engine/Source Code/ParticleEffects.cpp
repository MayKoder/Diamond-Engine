#include "ParticleEffects.h"
#include "Particle.h"

ParticleEffect::ParticleEffect(PARTICLE_EFFECT_TYPE type):type(type)
{
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::Spawn(Particle& particle)
{
}

void ParticleEffect::Update(Particle& particle, float dt)
{
	
}

#ifndef STANDALONE
void ParticleEffect::OnEditor(int emitterIndex)
{
}
#endif // !STANDALONE