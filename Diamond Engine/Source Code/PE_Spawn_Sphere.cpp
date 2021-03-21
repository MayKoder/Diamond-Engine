#include "PE_Spawn_Sphere.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnSphere::PE_SpawnSphere() :ParticleEffect(PARTICLE_EFFECT_TYPE::AREA_SPAWN_SPHERE)
{
	memset(centerOfSphere, 0.f, sizeof(centerOfSphere));
	radius = 1.0f;
}

PE_SpawnSphere::~PE_SpawnSphere()
{

}

void PE_SpawnSphere::Spawn(Particle& particle)
{
	//Get a random spawn point inside of a quad defined by a point and a radius
	float u = EngineExternal->GetRandomFloat(-radius, radius);
	float x1 = EngineExternal->GetRandomFloat(-radius, radius);
	float x2 = EngineExternal->GetRandomFloat(-radius, radius);
	float x3 = EngineExternal->GetRandomFloat(-radius, radius);

	float mag = sqrt(x1*x1 + x2*x2 + x3*x3);
	x1 /= mag; x2 /= mag; x3 /= mag;
	float c = cbrt(u);
	particle.pos.x += x1*c;
	particle.pos.y += x2*c;
	particle.pos.z += x3*c;
}

#ifndef STANDALONE
void PE_SpawnSphere::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Spawn in Sphere Area Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete Spawn in Sphere Area Effect"))
			this->toDelete = true;

		suffixLabel = "Offset##lPaSpdSphereSpawn";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), centerOfSphere);
		suffixLabel = "Radius##lPaSpdSphereRadius";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &radius);
	}
}
#endif // !STANDALONE


void PE_SpawnSphere::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteVector3(nObj, "paSpawnAreaPos", centerOfSphere);
	DEJson::WriteFloat(nObj, "paSpawnAreaRadius", radius);
}


void PE_SpawnSphere::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	float3 pos = nObj.ReadVector3("paSpawnAreaPos");
	centerOfSphere[0] = pos.x;
	centerOfSphere[1] = pos.y;
	centerOfSphere[2] = pos.z;

	radius = nObj.ReadFloat("paSpawnAreaRadius");
}