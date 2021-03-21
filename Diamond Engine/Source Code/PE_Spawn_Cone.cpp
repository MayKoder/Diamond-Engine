#include "PE_Spawn_Cone.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnCone::PE_SpawnCone() :ParticleEffect(PARTICLE_EFFECT_TYPE::AREA_SPAWN_CONE)
{
	memset(coneTip, 0.f, sizeof(coneTip));
	memset(centerOfSphere, 10.f, sizeof(coneTip));
	radius = 1.0f;
}

PE_SpawnCone::~PE_SpawnCone()
{

}

void PE_SpawnCone::Spawn(Particle& particle)
{
	//Particles spawn at the cone tip and go upwards

	//TODO: Cone
	float direction[3];
	direction[0] = centerOfSphere[0] - coneTip[0];
	direction[1] = centerOfSphere[1] - coneTip[1];
	direction[2] = centerOfSphere[2] - coneTip[2];

	//float aux = EngineExternal->GetRandomFloat(-radius, radius);

	particle.speed.x += direction[0];
	particle.speed.y += direction[1];
	particle.speed.z += direction[2];
}

#ifndef STANDALONE
void PE_SpawnCone::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Spawn in Cone Area Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete Spawn in Cone Area Effect"))
			this->toDelete = true;

		suffixLabel = "Center of Sphere";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), coneTip);
		suffixLabel = "Radius##lPaSpdConeRadius";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &radius);
	}
}
#endif // !STANDALONE


void PE_SpawnCone::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteVector3(nObj, "paSpawnAreaPos", coneTip);
	DEJson::WriteFloat(nObj, "paSpawnAreaRadius", radius);
}


void PE_SpawnCone::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	float3 pos = nObj.ReadVector3("paSpawnAreaPos");
	coneTip[0] = pos.x;
	coneTip[1] = pos.y;
	coneTip[2] = pos.z;

	radius = nObj.ReadFloat("paSpawnAreaRadius");
}