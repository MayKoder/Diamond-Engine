#pragma once
#include "Component.h"

class ResourceMaterial;
class C_Script;

class C_Button :public Component {
public:
	C_Button(GameObject* gameObject);
	~C_Button() override;

	void ChangeSprite(int num_sprite, ResourceMaterial* sprite);
	void ChangeScript(C_Script* script);

	void ExecuteButton();
	void ReleaseButton();


#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

private:
	ResourceMaterial* sprite1;
	ResourceMaterial* sprite2;
	ResourceMaterial* sprite3;
	C_Script* script;
};