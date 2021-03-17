#pragma once
#include "Component.h"

class ResourceTexture;
class C_AudioSource;

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

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

#ifndef STANDALONE
	void ChangeSprite(BUTTONSTATE num_sprite, ResourceTexture* sprite);
	void ChangeScript(const char* script_name);
	bool OnEditor() override;

private:
	bool sprites_freezed;
#endif // !STANDALONE

public:
	bool is_selected;
private:
	ResourceTexture* sprite_button_pressed;
	ResourceTexture* sprite_button_hovered;
	ResourceTexture* sprite_button_unhovered;
	BUTTONSTATE num_sprite_used;
	std::string script_name;
	C_AudioSource* thisAudSource;
};