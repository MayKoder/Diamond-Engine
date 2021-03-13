#pragma once

#include "ParticleEffects.h";

class PE_ColorOverLifetime : public ParticleEffect
{
public:
	PE_ColorOverLifetime();
	~PE_ColorOverLifetime();

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void Update(Particle& particle, float dt) override;
private:
	float startingColor[4]; //TODO change this to a gradient 
	float endingColor[4];
};