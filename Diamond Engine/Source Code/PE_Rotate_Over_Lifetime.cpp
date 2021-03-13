#include "PE_Rotate_Over_Lifetime.h"
#include "Particle.h"
#include "Application.h"
#include "ImGui/imgui.h"

PE_RotateOverLifetime::PE_RotateOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::ROTATE_OVER_LIFETIME)
{
	rotation[0] = 0.0f;
	rotation[1] = 0.0f;
	rotationSpeed[0] = -1.0f;
	rotationSpeed[1] = 1.0f;

}

PE_RotateOverLifetime::~PE_RotateOverLifetime()
{
}

void PE_RotateOverLifetime::Spawn(Particle& particle)
{

	particle.rotation = EngineExternal->GetRandomFloat(rotation[0], rotation[1]);
	particle.rotationSpeed = EngineExternal->GetRandomFloat(rotationSpeed[0], rotationSpeed[1]);
}

void PE_RotateOverLifetime::Update(Particle& particle, float dt)
{
	particle.rotation += particle.rotationSpeed * dt;
}

void PE_RotateOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Rotate Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		suffixLabel = "Initial Rotation##";
		suffixLabel += emitterIndex;
		ImGui::DragFloat2(suffixLabel.c_str(), rotation);

		suffixLabel = "Rotation Speed##";
		suffixLabel += emitterIndex;
		ImGui::DragFloat2(suffixLabel.c_str(), rotationSpeed);
	}
}
