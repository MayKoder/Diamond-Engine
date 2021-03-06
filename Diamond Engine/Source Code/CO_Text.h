#pragma once
#include "Component.h"

#include "MathGeoLib/include/MathGeoLibFwd.h"

#define MAX_CHARACTER_TEXT 500

class ResourceMaterial;
class C_Transform2D;
class FontDictionary;
struct Character;

class C_Text :public Component {
public:
	C_Text(GameObject* gameObject);
	~C_Text() override;

	void RenderText(C_Transform2D* transform, ResourceMaterial* material, unsigned int VAO, unsigned int VBO);

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

private:
	const char* font_path;
	std::string text;

	float textColor[3];

	FontDictionary* font = nullptr;
};