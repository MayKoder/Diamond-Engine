#include "PE_Force_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_ForceOverLifetime::PE_ForceOverLifetime() : ParticleEffect(PARTICLE_EFFECT_TYPE::FORCE_OVER_LIFETIME)
{
	memset(acceleration, 0.f, sizeof(acceleration));

	myGravity =- 9.8f;
	gravityModifier = 1.0f;
}


PE_ForceOverLifetime::~PE_ForceOverLifetime()
{
}


void PE_ForceOverLifetime::Update(Particle& particle, float dt)
{
	particle.accel.x = acceleration[0];
	particle.accel.y = acceleration[1]+ myGravity;
	particle.accel.z = acceleration[2];
}


#ifndef STANDALONE
void PE_ForceOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Force Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete Force Effect"))
			this->toDelete = true;

		suffixLabel = "Force Vector##DirectionalForce";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), acceleration, 0.1f); 
		
		suffixLabel = "Gravity Modifier##GravityForce";
		suffixLabel += emitterIndex;
		if (ImGui::DragFloat(suffixLabel.c_str(), &gravityModifier, 0.1f))
		{
			myGravity = -9.8f * gravityModifier;
		}

	}
}
#endif // !STANDALONE


void PE_ForceOverLifetime::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteVector3(nObj, "paFOLT_acceleration", acceleration);
	DEJson::WriteFloat(nObj, "paFOLT_gravityMod", gravityModifier);
	DEJson::WriteFloat(nObj, "paFOLT_gravity", myGravity);
}


void PE_ForceOverLifetime::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	float3 acc = nObj.ReadVector3("paFOLT_acceleration");
	acceleration[0] = acc.x;
	acceleration[1] = acc.y;
	acceleration[2] = acc.z;

	gravityModifier = nObj.ReadFloat("paFOLT_gravityMod");
	myGravity = nObj.ReadFloat("paFOLT_gravity");
}