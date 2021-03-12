#include "PE_Fade.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_Fade::PE_Fade():ParticleEffect(PARTICLE_EFFECT_TYPE::FADE)
{
}

PE_Fade::~PE_Fade()
{
}

#ifndef STANDALONE
void PE_Fade::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Fade Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{

	}
}
#endif // !STANDALONE