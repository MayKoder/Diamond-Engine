#pragma once	
#include "Component.h"

class C_Transform;

class C_AudioListener : public Component
{
public:
	C_AudioListener(GameObject* _gm,bool defaultListener=false);
	virtual ~C_AudioListener();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	float GetVolume();
	void SetVolume(float newVol);

	uint GetID();
	void SetID(uint id);

	void SetAsDefaultListener(bool setDefault = true);
	bool IsDefaultListener();

private:

	C_Transform* myTransform;
	bool isDefaultListener;
	unsigned int id;
	float masterVolume;
};
