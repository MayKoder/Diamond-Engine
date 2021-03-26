#include "PE_Spawn_Sphere.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnShapeSphere::PE_SpawnShapeSphere() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::SPHERE)
{
	radius = 1.0f;
}

PE_SpawnShapeSphere::~PE_SpawnShapeSphere()
{

}

void PE_SpawnShapeSphere::Spawn(Particle& particle, bool hasInitialSpeed, float speed, float4x4& gTrans, float* offset)
{
	//Get a random spawn point inside of a quad defined by a point and a radius
	float u = EngineExternal->GetRandomFloat(-radius, radius);
	float x1 = EngineExternal->GetRandomFloat(-radius, radius);
	float x2 = EngineExternal->GetRandomFloat(-radius, radius);
	float x3 = EngineExternal->GetRandomFloat(-radius, radius);

	float mag = sqrt(x1 * x1 + x2 * x2 + x3 * x3);
	x1 /= mag; x2 /= mag; x3 /= mag;
	float c = cbrt(u);
	float3 localPos;
	localPos.x = offset[0] + x1 * c;
	localPos.y = offset[1] + x2 * c;
	localPos.z = offset[2] + x3 * c;
	particle.pos = gTrans.TransformPos(localPos);

	if (hasInitialSpeed)
	{
		float3 localSpeed = (localPos - float3(offset[0], offset[1], offset[2])).Normalized() * speed;
		particle.speed = gTrans.TransformDir(localSpeed).Normalized() * speed;
	}
}

#ifndef STANDALONE
void PE_SpawnShapeSphere::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Radius##PaShapeSphere";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeSphere::SaveData(JSON_Object* nObj)
{
	DEJson::WriteFloat(nObj, "PaShapeSphereRadius", radius);
}


void PE_SpawnShapeSphere::LoadData(DEConfig& nObj)
{
	radius = nObj.ReadFloat("PaShapeSphereRadius");
}