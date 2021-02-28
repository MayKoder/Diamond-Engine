#pragma once
#include "Component.h"

class ResourceMaterial;
class C_Script;

class C_Button :public Component {
public:
	C_Button(GameObject* gameObject);
	~C_Button() override;

	void Update() override;

	void ExecuteButton();
	void ReleaseButton();

	void ChangeMaterial(int new_num_sprite);

#ifndef STANDALONE
	void ChangeSprite(int num_sprite, ResourceMaterial* sprite);
	void ChangeScript(C_Script* script);
	bool OnEditor() override;
#endif // !STANDALONE

private:
	ResourceMaterial* sprite1;
	ResourceMaterial* sprite2;
	ResourceMaterial* sprite3;
	int num_sprite_used;
	C_Script* script;
};