#include "ParticleEffects.h"

ParticleEffect::ParticleEffect(PARTICLEEFFECTTYPES type):type(type)
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