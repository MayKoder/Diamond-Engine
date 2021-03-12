#include "PE_Spawn_Area.h"
#include "Particle.h"
#include "Application.h"

#include "ImGui/imgui.h"

PE_Spawn_Area::PE_Spawn_Area():ParticleEffect(PARTICLE_EFFECT_TYPE::AREA_SPAWN)
{
	memset(centerOfSphere, 0.f, sizeof(centerOfSphere));
}

PE_Spawn_Area::~PE_Spawn_Area()
{
}

void PE_Spawn_Area::Spawn(Particle& particle)
{
	//Get a random spawn point inside of a sphere defined by a point and a radius
	particle.pos.x = centerOfSphere[0] + EngineExternal->GetRandomFloat(-radius, radius);
	particle.pos.y = centerOfSphere[1] + EngineExternal->GetRandomFloat(-radius, radius);
	particle.pos.z = centerOfSphere[2] + EngineExternal->GetRandomFloat(-radius, radius);
}

#ifndef STANDALONE
void PE_Spawn_Area::OnEditor(int emitterIndex)
{
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Particle movement: ");

	int offset = ImGui::CalcTextSize("Particle speed: ").x + 16;
	ImGui::Text("Particle speed: ");
	ImGui::SameLine();
	ImGui::DragFloat3("##lPaSpd", centerOfSphere, 0.1f);
}
#endif // !STANDALONE