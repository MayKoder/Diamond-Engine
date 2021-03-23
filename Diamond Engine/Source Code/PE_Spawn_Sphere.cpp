#include "PE_Spawn_Sphere.h"
#include "Particle.h"
#include "Application.h"
#include "CO_Transform.h"
#include <math.h>

#include "ImGui/imgui.h"

PE_SpawnShapeSphere::PE_SpawnShapeSphere() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::SPHERE)
{
	memset(centerOfSphere, 0.f, sizeof(centerOfSphere));
	radius = 1.0f;
}

PE_SpawnShapeSphere::~PE_SpawnShapeSphere()
{

}

void PE_SpawnShapeSphere::Spawn(Particle& particle, bool hasInitialSpeed, float speed)
{
	//Get a random spawn point inside of a quad defined by a point and a radius
	float u = EngineExternal->GetRandomFloat(-radius, radius);
	float x1 = EngineExternal->GetRandomFloat(-radius, radius);
	float x2 = EngineExternal->GetRandomFloat(-radius, radius);
	float x3 = EngineExternal->GetRandomFloat(-radius, radius);

	float mag = sqrt(x1 * x1 + x2 * x2 + x3 * x3);
	x1 /= mag; x2 /= mag; x3 /= mag;
	float c = cbrt(u);
	particle.pos.x = centerOfSphere[0] + x1 * c;
	particle.pos.y = centerOfSphere[1] + x2 * c;
	particle.pos.z = centerOfSphere[2] + x3 * c;

	if (hasInitialSpeed)
	{
		particle.speed = (particle.pos - float3(centerOfSphere[0], centerOfSphere[1], centerOfSphere[2])).Normalized() * speed;
	}
}

#ifndef STANDALONE
void PE_SpawnShapeSphere::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Offset##PaShapeSphere";
	suffixLabel += emitterIndex;
	ImGui::DragFloat3(suffixLabel.c_str(), centerOfSphere);
	suffixLabel = "Radius##PaShapeSphere";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeSphere::SaveData(JSON_Object* nObj)
{
	DEJson::WriteVector3(nObj, "PaShapeSpherePos", centerOfSphere);
	DEJson::WriteFloat(nObj, "PaShapeSphereRadius", radius);
}


void PE_SpawnShapeSphere::LoadData(DEConfig& nObj)
{
	float3 pos = nObj.ReadVector3("PaShapeSpherePos");
	centerOfSphere[0] = pos.x;
	centerOfSphere[1] = pos.y;
	centerOfSphere[2] = pos.z;

	radius = nObj.ReadFloat("PaShapeSphereRadius");
}