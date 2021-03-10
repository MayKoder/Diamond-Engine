#pragma once
#include "Component.h"

class ResourceTexture;

enum class CHECKBOXSTATE {
	CHECKBOXACTIVE,
	CHECKBOXACTIVEHOVERED,
	CHECKBOXACTIVEPRESSED,
	CHECKBOXUNACTIVE,
	CHECKBOXUNACTIVEHOVERED,
	CHECKBOXUNACTIVEPRESSED
};

class C_Checkbox :public Component {
public:
	C_Checkbox(GameObject* gameObject);
	~C_Checkbox() override;

	void Update() override;

	void PressCheckbox();
	void UnpressCheckbox();

	void ChangeTexture(CHECKBOXSTATE new_num_sprite);

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

#ifndef STANDALONE
	void ChangeSprite(CHECKBOXSTATE num_sprite, ResourceTexture* sprite);
	void ChangeScript(const char* script);
	bool OnEditor() override;


private:
	bool sprites_freezed;
#endif // !STANDALONE

public:
	bool is_selected;
private:
	ResourceTexture* sprite_checkbox_active;
	ResourceTexture* sprite_checkbox_active_hovered;
	ResourceTexture* sprite_checkbox_active_pressed;
	ResourceTexture* sprite_checkbox_unactive;
	ResourceTexture* sprite_checkbox_unactive_hovered;
	ResourceTexture* sprite_checkbox_unactive_pressed;
	CHECKBOXSTATE num_sprite_used;
	std::string script_name;
	bool checkbox_active;
};