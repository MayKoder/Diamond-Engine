#pragma once
#include "Component.h"

class ResourceTexture;
class C_Script;

class C_Button :public Component {
public:
	C_Button(GameObject* gameObject);
	~C_Button() override;

	void Update() override;

	void ExecuteButton();
	void ReleaseButton();

	void ChangeTexture(int new_num_sprite);

#ifndef STANDALONE
	void ChangeSprite(int num_sprite, ResourceTexture* sprite);
	void ChangeScript(C_Script* script);
	bool OnEditor() override;
#endif // !STANDALONE

private:
	ResourceTexture* sprite1;
	ResourceTexture* sprite2;
	ResourceTexture* sprite3;
	int num_sprite_used;
	C_Script* script;
};