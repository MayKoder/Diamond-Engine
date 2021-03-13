#include "PE_Color_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_ColorOverLifetime::PE_ColorOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME)
{
	startingColor[0] = 1.0f;
	startingColor[1] = 1.0f;
	startingColor[2] = 1.0f;
	startingColor[3] = 1.0f;

	endingColor[0] = 0.0f;
	endingColor[1] = 0.0f;
	endingColor[2] = 0.0f;
	endingColor[3] = 0.0f;

}

PE_ColorOverLifetime::~PE_ColorOverLifetime()
{
}

#ifndef STANDALONE
void PE_ColorOverLifetime::OnEditor(int emitterIndex)
{

	std::string suffixLabel = "Color Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		suffixLabel = "Starting Color##ColorEdit";
		suffixLabel += emitterIndex;
		ImGui::ColorEdit4(suffixLabel.c_str(), startingColor);

		suffixLabel = "End Color##ColorEdit";
		suffixLabel += emitterIndex;
		ImGui::ColorEdit4(suffixLabel.c_str(), endingColor);
	}

}
void PE_ColorOverLifetime::Update(Particle& particle, float dt)
{
	//Linear downgrade - The particle's visibility goes down linearly with time
	if (particle.maxLifetime != 0.0f)
	{
		float percentatge = particle.currentLifetime / particle.maxLifetime;
		float4 startColor = { startingColor[0] ,startingColor[1] ,startingColor[2] ,startingColor[3] };
		float4 endColor = { endingColor[0] ,endingColor[1] ,endingColor[2] ,endingColor[3] };

		particle.color = (1 - percentatge) * endColor + percentatge * startColor;
	}
}
#endif // !STANDALONE