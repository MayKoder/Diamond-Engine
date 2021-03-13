#include "PE_Spawn_Area.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"

#include "ImGui/imgui.h"

PE_SpawnArea::PE_SpawnArea() :ParticleEffect(PARTICLE_EFFECT_TYPE::AREA_SPAWN)
{
	memset(centerOfSphere, 0.f, sizeof(centerOfSphere));
	radius = 1.0f;
}

PE_SpawnArea::~PE_SpawnArea()
{

}

void PE_SpawnArea::Spawn(Particle& particle)
{
	//Get a random spawn point inside of a sphere defined by a point and a radius
	particle.pos.x += centerOfSphere[0] + EngineExternal->GetRandomFloat(-radius, radius);//TODO we take particle.pos. as an imput for the moment because we set the position to the transform origin when we spawn it BUT this won't work when we have different effect.spawn that change postitions
	particle.pos.y += centerOfSphere[1] + EngineExternal->GetRandomFloat(-radius, radius);
	particle.pos.z += centerOfSphere[2] + EngineExternal->GetRandomFloat(-radius, radius);
}

#ifndef STANDALONE
void PE_SpawnArea::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Area Spawn Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		//ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Particle movement: ");

		/*int offset = ImGui::CalcTextSize("Particle speed: ").x + 16;
		ImGui::Text("Particle speed: ");
		ImGui::SameLine();
		suffixLabel = "##lPaSpdAreaSpawn";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), centerOfSphere, 0.1f);*/
		suffixLabel = "Offset##lPaSpdAreaSpawn";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), centerOfSphere);
		suffixLabel = "Radius##lPaSpdAreaRadius";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &radius);
	}
}
#endif // !STANDALONE