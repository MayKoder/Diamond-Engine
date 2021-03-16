#pragma once

#include "ParticleEffects.h"

class PE_RotateOverLifetime: public ParticleEffect
{
public:
	PE_RotateOverLifetime();
	~PE_RotateOverLifetime() override;

	void Spawn(Particle& particle) override;
	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif //!STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

private:
	float rotation[2];
	float rotationSpeed[2];
};