#include "ParticleEffects.h"

ParticleEffect::ParticleEffect(PARTICLE_EFFECT_TYPE type):type(type)
{
}

ParticleEffect::~ParticleEffect()
{
}

#ifndef STANDALONE
void ParticleEffect::Update(Particle& particle, float dt)
{
}
void ParticleEffect::OnEditor(int emitterIndex)
{
}
#endif // !STANDALONE