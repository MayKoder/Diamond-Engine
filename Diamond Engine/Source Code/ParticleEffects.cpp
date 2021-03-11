#include "ParticleEffects.h"

ParticleEffect::ParticleEffect(PARTICLE_EFFECT_TYPE type):type(type)
{
}

ParticleEffect::~ParticleEffect()
{
}

#ifndef STANDALONE
void ParticleEffect::OnEditor()
{
}
#endif // !STANDALONE