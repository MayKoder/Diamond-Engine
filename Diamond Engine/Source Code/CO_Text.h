#pragma once
#include "Component.h"


class C_Text :public Component {
public:
	C_Text(GameObject* gameObject);
	~C_Text() override;


#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

private:
	int font_id;
	const char* font_path;
};