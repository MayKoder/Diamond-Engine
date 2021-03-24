#include "PE_Spawn_Cone.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnShapeCone::PE_SpawnShapeCone() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::CONE)
{
	memset(conePos, 0.0f, sizeof(conePos));
	height = 1.0f;
	radius = 1.0f;
}

PE_SpawnShapeCone::~PE_SpawnShapeCone()
{

}

void PE_SpawnShapeCone::Spawn(Particle& particle, bool hasInitialSpeed, float speed)
{
	//TODO: CHANGE SPAWN ALGORITHM TO NOT EAT THE PERFORMANCE FOR BREAKFAST

	float h = height * EngineExternal->GetRandomFloat(0.0f, 1.0f);
	
	float r = (radius / height) * h * sqrt(EngineExternal->GetRandomFloat(0.0f, 1.0f));

	float t = 2 * PI * EngineExternal->GetRandomFloat(0.0f, 1.0f);

	particle.pos.x = r * cos(t);
	particle.pos.y = h;
	particle.pos.z = r * sin(t);

	if (hasInitialSpeed)
	{
		particle.speed = (particle.pos - float3(conePos[0], conePos[1], conePos[2])).Normalized() * speed;
	}
}

#ifndef STANDALONE
void PE_SpawnShapeCone::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Cone Position##PaShapeCone"; //TODO consider putting 
	suffixLabel += emitterIndex;
	ImGui::DragFloat3(suffixLabel.c_str(), conePos);

	suffixLabel = "Cone Height##PaShapeCone"; //TODO consider putting 
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &height);

	suffixLabel = "Cone Radius##PaShapeCone";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeCone::SaveData(JSON_Object* nObj)
{
	DEJson::WriteVector3(nObj, "PaShapeConePos", conePos);
	DEJson::WriteFloat(nObj, "PaShapeConeRadius", radius);
	DEJson::WriteFloat(nObj, "PaShapeConeHeight", height);
}


void PE_SpawnShapeCone::LoadData(DEConfig& nObj)
{
	float3 pos = nObj.ReadVector3("PaShapeConePos");
	conePos[0] = pos.x;
	conePos[1] = pos.y;
	conePos[2] = pos.z;

	radius = nObj.ReadFloat("PaShapeConeRadius");
	height = nObj.ReadFloat("PaShapeConeHeight");
}