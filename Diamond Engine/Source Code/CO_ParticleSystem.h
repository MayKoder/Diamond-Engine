#pragma once	
#include "Component.h"
#include "ParticleEmitter.h"

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


	void AddEmitter();
private:

	bool systemActive;
	//emitter vector
	std::vector<Emitter> myEmitters;
};