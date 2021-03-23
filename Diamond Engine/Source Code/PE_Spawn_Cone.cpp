#include "PE_Spawn_Cone.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnShapeCone::PE_SpawnShapeCone() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::CONE)
{
	memset(coneTip, 0.f, sizeof(coneTip));
	memset(centerOfSphere, 10.f, sizeof(coneTip));
	radius = 1.0f;
}

PE_SpawnShapeCone::~PE_SpawnShapeCone()
{

}

void PE_SpawnShapeCone::Spawn(Particle& particle, bool hasInitialSpeed, float speed)
{
	//TODO: MAKE THE CONE SPAWN THINGS!
	//float direction[3];
	//direction[0] = (centerOfSphere[0] - coneTip[0]);
	//direction[1] = (centerOfSphere[1] - coneTip[1]);
	//direction[2] = (centerOfSphere[2] - coneTip[2]);

	/*if (hasInitialSpeed)
	{
		particle.speed = (particle.pos - float3(centerOfSphere[0], centerOfSphere[1], centerOfSphere[2])).Normalized() * speed;
	}*/
}

#ifndef STANDALONE
void PE_SpawnShapeCone::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Center of Sphere##PaShapeArea";
	suffixLabel += emitterIndex;
	ImGui::DragFloat3(suffixLabel.c_str(), coneTip);

	suffixLabel = "Radius##PaShapeArea";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeCone::SaveData(JSON_Object* nObj)
{
	DEJson::WriteVector3(nObj, "PaShapeConePos", coneTip);
	DEJson::WriteFloat(nObj, "PaShapeConeRadius", radius);
}


void PE_SpawnShapeCone::LoadData(DEConfig& nObj)
{
	float3 pos = nObj.ReadVector3("PaShapeConePos");
	coneTip[0] = pos.x;
	coneTip[1] = pos.y;
	coneTip[2] = pos.z;

	radius = nObj.ReadFloat("PaShapeConeRadius");
}