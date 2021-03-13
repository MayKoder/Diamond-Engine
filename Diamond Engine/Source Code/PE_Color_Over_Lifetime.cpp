#include "PE_Color_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_ColorOverLifetime::PE_ColorOverLifetime():ParticleEffect(PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME)
{
}

PE_ColorOverLifetime::~PE_ColorOverLifetime()
{
}

#ifndef STANDALONE
void PE_ColorOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Fade Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{

	}
}
#endif // !STANDALONE