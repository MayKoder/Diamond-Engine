#include "PE_Move.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_Move::PE_Move() : ParticleEffect(PARTICLE_EFFECT_TYPE::MOVE)
{
	memset(speed, 0.f, sizeof(speed));
	memset(acceleration, 0.f, sizeof(acceleration));
}


PE_Move::~PE_Move()
{
}


void PE_Move::Spawn(Particle& particle)
{
	particle.speed.x = speed[0];
	particle.speed.y = speed[1];
	particle.speed.z = speed[2];
}


void PE_Move::Update(Particle& particle, float dt)
{
	particle.speed.x += acceleration[0];
	particle.speed.y += acceleration[1];
	particle.speed.z += acceleration[2];

	particle.pos.x += particle.speed.x;
	particle.pos.y += particle.speed.y;
	particle.pos.z += particle.speed.z;
}


#ifndef STANDALONE
void PE_Move::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Move Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{


		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Particle movement: ");

		int offset = ImGui::CalcTextSize("Particle speed: ").x + 16;
		ImGui::Text("Particle speed: ");
		ImGui::SameLine();
		suffixLabel = "##lPaSpdMoveEffect";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), speed, 0.1f);

		ImGui::Text("Part acceleration: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		suffixLabel = "##lPaAccMoveEffect";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), acceleration, 0.1f);
	}
}
#endif // !STANDALONE