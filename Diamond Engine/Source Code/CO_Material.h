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


	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	bool viewWithCheckers;
	ResourceTexture* matTexture;

};