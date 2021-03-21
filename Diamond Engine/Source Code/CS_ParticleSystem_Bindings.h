#pragma once

#include "Application.h"
#include "MO_MonoManager.h"
#include "CO_ParticleSystem.h"
#include "GameObject.h"
#include"CS_Transform_Bindings.h"

//TODO put this into a namespace to have the names normalized?
void PlayParticles(MonoObject* goObj)
{
	//GameObject* gameObject = EngineExternal->moduleMono->GameObject_From_CSGO(goObj);
	//C_ParticleSystem* particleSystem = dynamic_cast<C_ParticleSystem*>(gameObject->GetComponent(Component::TYPE::PARTICLE_SYSTEM));

	C_ParticleSystem* particleSystem = DECS_CompToComp<C_ParticleSystem*>(goObj);

	if (particleSystem != nullptr)
	{
		particleSystem->Play();
	}
}

void StopParticles(MonoObject* goObj)
{
	C_ParticleSystem* particleSystem = DECS_CompToComp<C_ParticleSystem*>(goObj);

	if (particleSystem != nullptr)
	{
		particleSystem->Stop();
	}
}

bool IsPlayingParticles(MonoObject* goObj)
{
	bool ret = false;

	C_ParticleSystem* particleSystem = DECS_CompToComp<C_ParticleSystem*>(goObj);

	if (particleSystem != nullptr)
	{
		ret = particleSystem->IsSystemActive();
	}
	return ret;
}

bool GetLoopingParticles(MonoObject* goObj)
{
	bool ret = false;

	C_ParticleSystem* particleSystem = DECS_CompToComp<C_ParticleSystem*>(goObj);

	if (particleSystem != nullptr)
	{
		ret= particleSystem->IsSystemLooped();
	}
	return ret;
}