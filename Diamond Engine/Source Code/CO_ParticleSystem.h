#pragma once	
#include "Component.h"
#include "ParticleEmitter.h"

#include "Timer.h"

class C_ParticleSystem : public Component
{
public:
	C_ParticleSystem(GameObject* _gm);
	virtual ~C_ParticleSystem();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void Update() override;
	void Draw();
	void SetActive(bool newActive);

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void Play();
	void Stop();

	void AddEmitter();
	bool IsSystemActive() const;
	bool IsSystemLooped() const;


private:

	bool systemActive;
	//emitter vector
	std::vector<Emitter*> myEmitters;

	bool looping = false;
	float maxDuration = 0.0f;
	Timer playTimer;
};