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

private:

};