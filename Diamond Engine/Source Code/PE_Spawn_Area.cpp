#include "PE_Spawn_Area.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"

#include "ImGui/imgui.h"

PE_SpawnArea::PE_SpawnArea() :ParticleEffect(PARTICLE_EFFECT_TYPE::AREA_SPAWN)
{
	memset(centerOfQuad, 0.f, sizeof(centerOfQuad));
	radius = 1.0f;
}

PE_SpawnArea::~PE_SpawnArea()
{

}

void PE_SpawnArea::Spawn(Particle& particle)
{
	//Get a random spawn point inside of a quad defined by a point and a radius
	particle.pos.x += centerOfQuad[0] + EngineExternal->GetRandomFloat(-radius, radius);//TODO we take particle.pos. as an imput for the moment because we set the position to the transform origin when we spawn it BUT this won't work when we have different effect.spawn that change postitions
	particle.pos.y += centerOfQuad[1] + EngineExternal->GetRandomFloat(-radius, radius);
	particle.pos.z += centerOfQuad[2] + EngineExternal->GetRandomFloat(-radius, radius);
}

#ifndef STANDALONE
void PE_SpawnArea::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Area Spawn Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete Area Spawn Effect"))
			this->toDelete = true;

		suffixLabel = "Offset##lPaSpdAreaSpawn";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), centerOfQuad);
		suffixLabel = "Radius##lPaSpdAreaRadius";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &radius);
	}
}
#endif // !STANDALONE


void PE_SpawnArea::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteVector3(nObj, "paSpawnAreaPos", centerOfQuad);
	DEJson::WriteFloat(nObj, "paSpawnAreaRadius", radius);
}


void PE_SpawnArea::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	float3 pos = nObj.ReadVector3("paSpawnAreaPos");
	centerOfQuad[0] = pos.x;
	centerOfQuad[1] = pos.y;
	centerOfQuad[2] = pos.z;

	radius = nObj.ReadFloat("paSpawnAreaRadius");
}