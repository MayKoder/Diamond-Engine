#pragma once
#include "Component.h"

#define MAX_TEXT_CONTENT 200

class C_Text :public Component {
public:
	C_Text(GameObject* gameObject);
	~C_Text() override;


#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

private:
	const char* font_path;
	char text_to_print[MAX_TEXT_CONTENT];
};