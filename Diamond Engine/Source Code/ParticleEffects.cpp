#include "ParticleEffects.h"
#include "Particle.h"

ParticleEffect::ParticleEffect(PARTICLE_EFFECT_TYPE type):type(type)
{
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::Update(Particle& particle, float dt)
{
	//Linear downgrade
	particle.color.w = particle.currentLifetime / particle.maxLifetime;
}

#ifndef STANDALONE
void ParticleEffect::OnEditor(int emitterIndex)
{
}
#endif // !STANDALONE