#include "ParticleEmitter.h"

Emitter::Emitter() : position(0.0f, 0.0f, 0.0f), particlesPerSec(0), lastParticeTime(0)//TODO do we need to pass values to the constructor?
{

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
void Emitter::OnEditor()
{
}
#endif // !STANDALONE

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
			//myEffects[i].Spawn(&myParticles[j]);
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
				//myEffects[i].Spawn(&myParticles[j]);
			}


			--numberOfParticlesToSpawn;
		}

		if (numberOfParticlesToSpawn == 0)
			break;
	}



}