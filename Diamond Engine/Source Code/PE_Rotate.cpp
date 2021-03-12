#include "PE_Rotate.h"
#include "Particle.h"

#include "ImGui/imgui.h"

#include "MathGeoLib/include/MathGeoLib.h"

PE_Rotate::PE_Rotate():ParticleEffect(PARTICLE_EFFECT_TYPE::ROTATE)
{
	rotationQuat = Quat(0, 0, 0, 1);
	rotationSpeed = 5.0f;
}

PE_Rotate::~PE_Rotate()
{
}

void PE_Rotate::Spawn(Particle& particle)
{
	particle.rotation = rotationQuat;
	particle.rotationSpeed = rotationSpeed;
}

void PE_Rotate::Update(Particle& particle, float dt)
{
}

void PE_Rotate::OnEditor(int emitterIndex)
{
}
