#pragma once
#include <vector>
#include "Particle.h"
#include "ParticleEffects.h"

class C_Transform;
class ResourceTexture;

#define MAX_PARTICLES 2000
#define INSTANCE_DATA_LENGHT 16

class Emitter
{
public:
	Emitter();
	~Emitter();

	void Update(float dt, bool systemActive);
	void Draw(unsigned int shaderId);

#ifndef STANDALONE
	void OnEditor(int emitterIndex);
#endif // !STANDALONE

	void AssignTransform(C_Transform* objTransform);
private:
	//calculates a new Pool Size from the particle spawn rate & particle lifeTime
	void CalculatePoolSize();
	void SetPoolSize(unsigned int poolSize);
	//adds particles to the pool
	void CreateParticles(unsigned int particlesToAdd);
	void ThrowParticles(float dt);
	//returns index if it exists, else returns -1
	int DoesEffectExist(PARTICLE_EFFECT_TYPE type);

	std::string ParticleEffectEnumToString(PARTICLE_EFFECT_TYPE type);
	void CreateEffect(PARTICLE_EFFECT_TYPE type);

	void PrepareParticleToSpawn(Particle& p, float3& startingPos);

	void PrepareVBO();

public:
	bool toDelete;
private:
	unsigned int VAO = 0u;
	unsigned int instanceVBO = 0u;
	unsigned int vertexVBO = 0u;
	ResourceTexture* texture = nullptr;
	
	//Min - Max Start particles lifeTime 
	float particlesLifeTime[2];

	//Min - Max Start particles speed 
	float particlesSpeed[2]; //TODO move to emisison shape component

	//Min - Max Start particles size 
	float particlesSize[2];

	//Particles Starting Color
	float4 particlesColor;

	//particles per second
	float particlesPerSec;
	//this variable holds the extra time from the particle spawn last frame
	float lastParticeTime;
	std::vector<Particle> myParticles;
	std::vector<ParticleEffect*> myEffects;
	C_Transform* objTransform;
};



const float particleVertices[] = {
-1, -1,
1, -1,
-1, 1,
1, -1,
1, 1,
-1, 1,
};