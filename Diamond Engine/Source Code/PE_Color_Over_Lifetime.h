#pragma once

#include "ParticleEffects.h";

#include "ImGui/imgui_color_gradient.h"

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
	ImGradient gradient;
	bool editGradient;
};