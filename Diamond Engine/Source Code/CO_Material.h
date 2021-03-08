#pragma once
#include "Component.h"
class ResourceTexture;
class ResourceMaterial;

class C_Material : public Component
{
public:
	C_Material(GameObject* _gm);
	virtual ~C_Material();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	static inline Type GetType() { return Type::Material; }; //This will allow us to get the type from a template

	int GetTextureID();


	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	bool viewWithCheckers;

	ResourceTexture* matTexture;
	ResourceMaterial* material;

};