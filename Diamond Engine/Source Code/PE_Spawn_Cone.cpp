#include "PE_Spawn_Cone.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnShapeCone::PE_SpawnShapeCone() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::CONE)
{
	height = 1.0f;
	radius = 1.0f;
}

PE_SpawnShapeCone::~PE_SpawnShapeCone()
{

}

void PE_SpawnShapeCone::Spawn(Particle& particle, bool hasInitialSpeed, float speed, float4x4& gTrans, float* offset)
{
	//TODO: CHANGE SPAWN ALGORITHM TO NOT EAT THE PERFORMANCE FOR BREAKFAST. UPDATE: this is not that much of a performance hit

	float h = height * EngineExternal->GetRandomFloat(0.0f, 1.0f);
	
	float r = (radius / height) * h * sqrt(EngineExternal->GetRandomFloat(0.0f, 1.0f));

	float t = 2 * PI * EngineExternal->GetRandomFloat(0.0f, 1.0f);
	
	float3 localPos;
	localPos.x = (r * cos(t))+ offset[0];
	localPos.y = h + offset[1];
	localPos.z = (r * sin(t))+ offset[2];
	particle.pos = gTrans.TransformPos(localPos);

	if (hasInitialSpeed)
	{
		float3 localSpeed = (localPos - float3(offset[0], offset[1], offset[2])).Normalized() * speed;
		particle.speed = gTrans.TransformDir(localSpeed).Normalized() * speed;
	}
}

#ifndef STANDALONE
void PE_SpawnShapeCone::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Cone Height##PaShapeCone"; //TODO consider putting 
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &height);

	suffixLabel = "Cone Radius##PaShapeCone";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeCone::SaveData(JSON_Object* nObj)
{
	DEJson::WriteFloat(nObj, "PaShapeConeRadius", radius);
	DEJson::WriteFloat(nObj, "PaShapeConeHeight", height);
}


void PE_SpawnShapeCone::LoadData(DEConfig& nObj)
{
	radius = nObj.ReadFloat("PaShapeConeRadius");
	height = nObj.ReadFloat("PaShapeConeHeight");
}