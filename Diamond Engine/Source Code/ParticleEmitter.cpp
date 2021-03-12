#include "ParticleEmitter.h"
#include "ImGui/imgui.h"
#include "PE_Spawn.h"
#include "PE_Move.h"


Emitter::Emitter() :
	particlesPerSec(0),
	lastParticeTime(0),
	toDelete(false)
{
	memset(particlesLifeTime, 0.0f, sizeof(particlesLifeTime));

	//create all effects here (initialized to nullptr)
	myEffects.resize((int)PARTICLE_EFFECT_TYPE::MAX, nullptr);
}

Emitter::~Emitter()
{

	for (int i = myEffects.size() - 1; i >= 0; --i)
	{
		if (myEffects[i] != nullptr)
		{
			delete(myEffects[i]);
			myEffects[i] = nullptr;
		}
		myEffects.erase(myEffects.begin() + i);
	}
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
	std::string guiName = "Emitter##" + emitterIndex;
	if (ImGui::CollapsingHeader(guiName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool poolToEdit = false;

		guiName = "Particle Lifetime##" + emitterIndex;
		if (ImGui::DragFloat2(guiName.c_str(), particlesLifeTime))
		{
			poolToEdit = true;
		}
		guiName = "Particles per Second##" + emitterIndex;
		if (ImGui::DragInt(guiName.c_str(), &particlesPerSec))
		{
			poolToEdit = true;
		}

		if (poolToEdit)
		{
			CalculatePoolSize();
		}


		for (int i = 0; i < (int)PARTICLE_EFFECT_TYPE::MAX; i++)
		{
			int index = DoesEffectExist((PARTICLE_EFFECT_TYPE)i);
			if (index != -1)
			{
				myEffects[index]->OnEditor(emitterIndex);
			}
			else
			{
				guiName = (ParticleEffectEnumToString((PARTICLE_EFFECT_TYPE)i)) + "##";
				guiName += emitterIndex;
				if (ImGui::Button(guiName.c_str()))
				{
					myEffects[i] = CreateEffect((PARTICLE_EFFECT_TYPE)i);
				}
			}
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

int Emitter::DoesEffectExist(PARTICLE_EFFECT_TYPE type)
{
	for (int i = 0; i < myEffects.size(); ++i)
	{
		if (myEffects[i]!=nullptr && myEffects[i]->type == type)
		{
			return i;
		}
	}

	return -1;
}

std::string Emitter::ParticleEffectEnumToString(PARTICLE_EFFECT_TYPE type)
{
	std::string ret = "";
	switch (type)
	{
	case PARTICLE_EFFECT_TYPE::NONE:
		break;
	case PARTICLE_EFFECT_TYPE::SPAWN:
		ret = "Spawn Effect";
		break;
	case PARTICLE_EFFECT_TYPE::AREA_SPAWN:
		ret = "Area Spawn Effect";
		break;
	case PARTICLE_EFFECT_TYPE::MOVE:
		ret = "Move Effect";
		break;
	case PARTICLE_EFFECT_TYPE::RANDOM_MOVE:
		ret = "Random Move Effect";
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE:
		ret = "Rotate Effect";
		break;
	case PARTICLE_EFFECT_TYPE::MAX:
		break;
	default:
		break;
	}
	return ret;
}

ParticleEffect* Emitter::CreateEffect(PARTICLE_EFFECT_TYPE type)
{
	ParticleEffect* newEffect=nullptr;

	switch (type)
	{
	case PARTICLE_EFFECT_TYPE::NONE:
		break;
	case PARTICLE_EFFECT_TYPE::SPAWN:
		newEffect = new PE_Spawn();
		break;
	case PARTICLE_EFFECT_TYPE::AREA_SPAWN:
		//TODO
		break;
	case PARTICLE_EFFECT_TYPE::MOVE:
		newEffect = new PE_Move();
		break;
	case PARTICLE_EFFECT_TYPE::RANDOM_MOVE:
		//TODO
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE:
		//TODO
		break;
	case PARTICLE_EFFECT_TYPE::MAX:
		//TODO
		break;
	default:
		break;
	}

	return newEffect;
}
