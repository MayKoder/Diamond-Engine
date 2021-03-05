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

private:
	float4x4 GetCharacterTransform(const Character& character, C_Transform2D* transform, float posX, float posY);

private:
	const char* font_path;
	std::string text;

	FontDictionary* font = nullptr;
};