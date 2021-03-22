#include "PE_Size_Over_Lifetime.h"
#include "Particle.h"
#include "ImGui/imgui.h"

PE_SizeOverLifetime::PE_SizeOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::SIZE_OVER_LIFETIME), startingSize(0.1f), endSize(1.0f)
{

}

PE_SizeOverLifetime::~PE_SizeOverLifetime()
{
}

void PE_SizeOverLifetime::Update(Particle& particle, float dt)
{

	if (particle.maxLifetime != 0.0f)
	{
		float percentatge = particle.currentLifetime / particle.maxLifetime;
		particle.size = (1 - percentatge) * endSize + percentatge * startingSize;
	}

}

#ifndef STANDALONE
void PE_SizeOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Size Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete Size Effect"))
			this->toDelete = true;

		suffixLabel = "Initial Size##SizeOverLife";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &startingSize, 0.1f, 0.0f, FLT_MAX);

		suffixLabel = "End Size##SizeOverLife";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &endSize, 0.1f, 0.0f, FLT_MAX);

	}

}
#endif // !STANDALONE


void PE_SizeOverLifetime::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteFloat(nObj, "paSizeOLTStartSize", startingSize);
	DEJson::WriteFloat(nObj, "paSizeOLTEndSize", endSize);
}


void PE_SizeOverLifetime::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);

	startingSize = nObj.ReadFloat("paSizeOLTStartSize");
	endSize = nObj.ReadFloat("paSizeOLTEndSize");
}