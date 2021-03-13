#include "PE_Color_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"


PE_ColorOverLifetime::PE_ColorOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME), editGradient(false)
{
}

PE_ColorOverLifetime::~PE_ColorOverLifetime()
{
	gradient.getMarks().clear();
}

#ifndef STANDALONE
void PE_ColorOverLifetime::OnEditor(int emitterIndex)
{

	std::string suffixLabel = "Color Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (editGradient)
		{
			suffixLabel = "Save Edited Gradient##ColorGradient";
			suffixLabel += emitterIndex;
			if(ImGui::Button(suffixLabel.c_str()))
			{
				editGradient = false;
			}

			static ImGradientMark* draggingMark = nullptr;
			static ImGradientMark* selectedMark = nullptr;

			ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

		}
		else
		{
			if (ImGui::GradientButton(&gradient))
			{
				editGradient = true;
			}
		}
	}

}
void PE_ColorOverLifetime::Update(Particle& particle, float dt)
{
	//Linear downgrade - The particle's visibility goes down linearly with time
	if (particle.maxLifetime != 0.0f)
	{
		float percentatge = particle.currentLifetime / particle.maxLifetime;
		float myColor[4];
		gradient.getColorAt(percentatge, myColor);
		particle.color = { myColor[0] ,myColor[1] ,myColor[2] ,myColor[3] };
	}
}
#endif // !STANDALONE