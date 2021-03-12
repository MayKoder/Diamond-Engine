#pragma once

#include "ParticleEffects.h";

class PE_Fade : public ParticleEffect
{
public:
	PE_Fade();
	~PE_Fade();

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

private:

};