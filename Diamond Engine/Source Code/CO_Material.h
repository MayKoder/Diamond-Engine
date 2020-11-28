#pragma once
#include "Component.h"
class ResourceTexture;

class C_Material : public Component
{
public:
	C_Material(GameObject* _gm);
	virtual ~C_Material();

	/*void Update() override;*/
	bool OnEditor() override;
	int GetTextureID();

	ResourceTexture* matTexture;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(JSON_Object* nObj) override;

	bool viewWithCheckers;

};