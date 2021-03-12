#include "PE_Spawn.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_Spawn::PE_Spawn() : ParticleEffect(PARTICLE_EFFECT_TYPE::SPAWN)
{
	memset(spawnPos, 0.f, sizeof(spawnPos));
}


PE_Spawn::~PE_Spawn()
{
}


void PE_Spawn::Spawn(Particle& particle)
{
	particle.pos.x = spawnPos[0];
	particle.pos.y = spawnPos[1];
	particle.pos.z = spawnPos[2];
}


#ifndef STANDALONE
void PE_Spawn::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Spawn Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{

		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Particle movement: ");

		int offset = ImGui::CalcTextSize("Particle speed: ").x + 16;
		ImGui::Text("Particle speed: ");
		ImGui::SameLine();
		suffixLabel = "##lPaSpdSpawn";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), spawnPos, 0.1f);
	}
}
#endif // !STANDALONE