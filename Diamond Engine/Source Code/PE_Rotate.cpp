#include "PE_Rotate.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_Rotate::PE_Rotate() :ParticleEffect(PARTICLE_EFFECT_TYPE::ROTATE)
{
	rotation = 0;
	rotationSpeed = 5.0f;
}

PE_Rotate::~PE_Rotate()
{
}

void PE_Rotate::Spawn(Particle& particle)
{
	particle.rotation = rotation;
	particle.rotationSpeed = rotationSpeed;
}

void PE_Rotate::Update(Particle& particle, float dt)
{
	particle.rotation += rotationSpeed * dt;
}

void PE_Rotate::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Rotate Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{

	}
}
