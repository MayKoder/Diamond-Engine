#pragma once

#include "ParticleEffects.h"

class PE_SizeOverLifetime : public ParticleEffect
{
public:
	PE_SizeOverLifetime();
	~PE_SizeOverLifetime() override;


	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif //!STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

private:
	//TODO change this with a spline that can be edited in the future?
	float startingSize;
	float endSize;
};