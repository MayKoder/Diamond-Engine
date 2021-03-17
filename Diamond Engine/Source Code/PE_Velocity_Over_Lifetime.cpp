#include "PE_Velocity_Over_Lifetime.h"
#include "Particle.h"
#include "ImGui/imgui.h"

PE_VelocityOverLifetime::PE_VelocityOverLifetime(): ParticleEffect(PARTICLE_EFFECT_TYPE::VELOCITY_OVER_LIFETIME)
{
	memset(velocity, 0.0f, sizeof(velocity));
}

PE_VelocityOverLifetime::~PE_VelocityOverLifetime()
{
}

void PE_VelocityOverLifetime::Spawn(Particle& particle)
{
	particle.speed = { velocity[0],velocity[1],velocity[2] };
}

void PE_VelocityOverLifetime::Update(Particle& particle, float dt)
{
	particle.pos += float3( velocity[0], velocity[1], velocity[2])*dt;//TODO shouldn't integrate vel into pos here, fix it
}


#ifndef STANDALONE
void PE_VelocityOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Velocity Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		suffixLabel = "Velocity##vel";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), velocity, 0.1f);
	}
}
#endif // !STANDALONE


void PE_VelocityOverLifetime::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);
	DEJson::WriteVector3(nObj, "paVelocityOLTVelocity", velocity);
}


void PE_VelocityOverLifetime::LoadData(DEConfig& nObj)
{
	float3 vel = nObj.ReadVector3("paVelocityOLTVelocity");
	velocity[0] = vel.x;
	velocity[1] = vel.y;
	velocity[2] = vel.z;
}