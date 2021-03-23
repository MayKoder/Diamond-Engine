#include "PE_Spawn_Area.h"
#include "Particle.h"
#include "Application.h"

#include "ImGui/imgui.h"

PE_SpawnShapeArea::PE_SpawnShapeArea() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::AREA)
{
	memset(centerOfQuad, 0.f, sizeof(centerOfQuad));
	radius = 1.0f;
}

PE_SpawnShapeArea::~PE_SpawnShapeArea()
{

}

void PE_SpawnShapeArea::Spawn(Particle& particle, bool hasInitialSpeed, float speed)
{
	//Get a random spawn point inside of a quad defined by a point and a radius
	particle.pos.x += centerOfQuad[0] + EngineExternal->GetRandomFloat(-radius, radius);//TODO we take particle.pos. as an imput for the moment because we set the position to the transform origin when we spawn it BUT this won't work when we have different effect.spawn that change postitions
	particle.pos.y += centerOfQuad[1] + EngineExternal->GetRandomFloat(-radius, radius);
	particle.pos.z += centerOfQuad[2] + EngineExternal->GetRandomFloat(-radius, radius);

	if (hasInitialSpeed)
	{
		particle.speed = (particle.pos - float3(centerOfQuad[0], centerOfQuad[1], centerOfQuad[2])).Normalized() * speed;
	}


}

#ifndef STANDALONE
void PE_SpawnShapeArea::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Offset##PaShapeArea";
	suffixLabel += emitterIndex;
	ImGui::DragFloat3(suffixLabel.c_str(), centerOfQuad);
	suffixLabel = "Radius##PaShapeArea";
	suffixLabel += emitterIndex;
	ImGui::DragFloat(suffixLabel.c_str(), &radius);

}
#endif // !STANDALONE


void PE_SpawnShapeArea::SaveData(JSON_Object* nObj)
{
	DEJson::WriteVector3(nObj, "PaShapeAreaPos", centerOfQuad);
	DEJson::WriteFloat(nObj, "PaShapeAreaRadius", radius);
}


void PE_SpawnShapeArea::LoadData(DEConfig& nObj)
{
	float3 pos = nObj.ReadVector3("PaShapeAreaPos");
	centerOfQuad[0] = pos.x;
	centerOfQuad[1] = pos.y;
	centerOfQuad[2] = pos.z;

	radius = nObj.ReadFloat("PaShapeAreaRadius");
}