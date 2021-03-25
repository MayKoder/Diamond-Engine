#include "PE_Spawn_Area.h"
#include "Particle.h"
#include "Application.h"

#include "MathGeoLib/include/Math/float4x4.h"

#include "ImGui/imgui.h"

PE_SpawnShapeArea::PE_SpawnShapeArea() :PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE::AREA)
{
	memset(dimensions, 1.f, sizeof(dimensions));
}

PE_SpawnShapeArea::~PE_SpawnShapeArea()
{

}

void PE_SpawnShapeArea::Spawn(Particle& particle, bool hasInitialSpeed, float speed, float4x4& gTrans, float* offset)
{

	//Get a random spawn point inside of a quad defined by a point and a radius
	
	float3 localPos;
	localPos.x = offset[0] + EngineExternal->GetRandomFloat(-dimensions[0], dimensions[0]);
	localPos.y = offset[1] + EngineExternal->GetRandomFloat(-dimensions[1], dimensions[1]);
	localPos.z = offset[2] + EngineExternal->GetRandomFloat(-dimensions[2], dimensions[2]);
	particle.pos = gTrans.TransformPos(localPos);

	if (hasInitialSpeed)
	{
		float3 localSpeed = (localPos - float3(offset[0], offset[1], offset[2])).Normalized();
		particle.speed = gTrans.TransformDir(localSpeed).Normalized() * speed;
	}


}

#ifndef STANDALONE
void PE_SpawnShapeArea::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Dimensions##PaShapeArea";
	suffixLabel += emitterIndex;
	ImGui::DragFloat3(suffixLabel.c_str(), dimensions);

}
#endif // !STANDALONE


void PE_SpawnShapeArea::SaveData(JSON_Object* nObj)
{
	DEJson::WriteVector3(nObj, "PaShapeAreaDimensions", dimensions);
}


void PE_SpawnShapeArea::LoadData(DEConfig& nObj)
{
	float3 newDimensions = nObj.ReadVector3("PaShapeAreaDimensions");
	dimensions[0] = newDimensions.x;
	dimensions[1] = newDimensions.y;
	dimensions[2] = newDimensions.z;
}