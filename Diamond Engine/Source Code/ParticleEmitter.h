#pragma once
#include <vector>
#include "Particle.h"
#include "ParticleEffects.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "DEJsonSupport.h"
#include "Timer.h"

class C_Transform;
class ResourceTexture;

#define MAX_PARTICLES 50000
#define INSTANCE_DATA_LENGHT 20

class Emitter
{
public:
	Emitter();
	~Emitter();

	void Update(float dt, bool systemActive);
	void Draw(unsigned int shaderId, Quat newRotation);

#ifndef STANDALONE
	void OnEditor(int emitterIndex);
	void TryDeleteUnusedEffects();
#endif // !STANDALONE

	void AssignTransform(C_Transform* objTransform);

	void SaveData(JSON_Object* nObj);
	void LoadData(DEConfig& nObj);

	void RestartEmitter();


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

	//void PrepareVBO();
	int FindUnusedParticle();
	void SetParticlesPerSec(int newParticlesPerSec);

	void AddInstancedAttribute(unsigned int vao, unsigned int vbo, int attributeIndex, int dataSize, int instancedDataLength, int offset);

public:
	bool toDelete;
private:
	unsigned int VAO = 0u;
	unsigned int instanceVBO = 0u;
	unsigned int vertexVBO = 0u;
	ResourceTexture* texture = nullptr;
	
	//Min - Max Start particles lifeTime 
	float particlesLifeTime[2];

	//Min - Max Start particles size 
	float particlesSize[2];

	//Particles Starting Color
	float particlesColor[4];

	//particles per second
	float particlesPerSec;
	//seconds per particle (1/particles per sec)
	float secPerParticle;
	//this variable holds the extra time from the particle spawn last frame
	float lastParticeTime;
	//index of the last used aprticle
	int lastUsedParticle;

	std::vector<Particle> myParticles;
	std::vector<ParticleEffect*> myEffects;
	C_Transform* objTransform = nullptr;

	bool delaying = false;
	float maxDelay;
	Timer delay;
	std::string emitterName;
	char emitterNameChars[50];
};

const float particleVertices[] = {
-1, -1,
1, -1,
-1, 1,
1, -1,
1, 1,
-1, 1,
};