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


void ParticleEffect::SaveData(JSON_Object* nObj)
{
	DEJson::WriteInt(nObj, "paEffectType", (int)type);
}


void ParticleEffect::LoadData(DEConfig& nObj)
{
}