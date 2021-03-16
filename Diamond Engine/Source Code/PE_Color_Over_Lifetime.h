#pragma once

#include "ParticleEffects.h";

#include "ImGui/imgui_color_gradient.h"

class PE_ColorOverLifetime : public ParticleEffect
{
public:
	PE_ColorOverLifetime();
	~PE_ColorOverLifetime();

	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

private:
	ImGradient gradient;
	bool editGradient;
};