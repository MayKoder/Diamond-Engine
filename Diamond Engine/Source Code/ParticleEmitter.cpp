#include "ParticleEmitter.h"
#include "ImGui/imgui.h"


Emitter::Emitter() : 
particlesPerSec(0), 
lastParticeTime(0),
toDelete(false)
{
	memset(particlesLifeTime, 0.0f, sizeof(particlesLifeTime));

}

Emitter::~Emitter()
{
}

void Emitter::Update(float dt, bool systemActive)
{

}

void Emitter::Draw()
{
}

#ifndef STANDALONE
void Emitter::OnEditor(int emitterIndex)
{
	std::string guiName = "Emitter " + emitterIndex;
	if (ImGui::CollapsingHeader(guiName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool poolToEdit = false;

		guiName = "Particle Lifetime##" + emitterIndex;
		if (ImGui::DragFloat2(guiName.c_str(), particlesLifeTime))
		{
			poolToEdit = true;
		}
		guiName = "Particles per Second##" + emitterIndex;
		if (ImGui::DragInt(guiName.c_str(),&particlesPerSec))
		{
			poolToEdit = true;
		}

		if (poolToEdit)
		{
			CalculatePoolSize();
		}

		for (int i = 0; i < myEffects.size(); ++i)
		{
			myEffects[i]->OnEditor(emitterIndex);

		}
	}
}
#endif // !STANDALONE

void Emitter::CalculatePoolSize()
{
	int poolSize = ceilf(particlesPerSec * particlesLifeTime[1]);
	SetPoolSize(poolSize);
}

void Emitter::SetPoolSize(unsigned int poolSize)
{
	myParticles.resize(poolSize);
}

void Emitter::CreateParticles(unsigned int particlesToAdd)
{
	unsigned int lastIndex = myParticles.size();
	myParticles.resize(lastIndex + particlesToAdd);
	for (int i = 0; i < myEffects.size(); ++i)
	{
		for (unsigned int j = lastIndex; j < myParticles.size(); ++j)
		{
			myEffects[i]->Spawn(myParticles[j]);
		}
	}
}

void Emitter::ThrowParticles(float dt)
{
	//find particles to spawn this frame
	float timeSinceLastThrow = dt + lastParticeTime;
	int numberOfParticlesToSpawn = timeSinceLastThrow * particlesPerSec;
	lastParticeTime = (timeSinceLastThrow * particlesPerSec) - numberOfParticlesToSpawn;


	//spawn particles
	for (int i = 0; i < myParticles.size(); ++i)
	{
		if (myParticles[i].currentLifetime < 0.0f)
		{
			for (int j = 0; j < myEffects.size(); ++j)
			{
				myEffects[i]->Spawn(myParticles[j]);
			}
			--numberOfParticlesToSpawn;
		}

		if (numberOfParticlesToSpawn == 0)
			break;
	}

}