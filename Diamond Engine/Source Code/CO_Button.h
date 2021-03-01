#pragma once
#include "Component.h"

class ResourceTexture;
class C_Script;

enum class BUTTONSTATE {
	BUTTONPRESSED,
	BUTTONHOVERED,
	BUTTONUNHOVERED
};

class C_Button :public Component {
public:
	C_Button(GameObject* gameObject);
	~C_Button() override;

	void Update() override;

	void ExecuteButton();
	void ReleaseButton();

	void ChangeTexture(BUTTONSTATE new_num_sprite);

#ifndef STANDALONE
	void ChangeSprite(BUTTONSTATE num_sprite, ResourceTexture* sprite);
	void ChangeScript(C_Script* script);
	bool OnEditor() override;
#endif // !STANDALONE

private:
	ResourceTexture* sprite_button_pressed;
	ResourceTexture* sprite_button_hovered;
	ResourceTexture* sprite_button_unhovered;
	BUTTONSTATE num_sprite_used;
	C_Script* script;
};