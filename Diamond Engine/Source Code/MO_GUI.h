#ifndef __MO_GUI_H__
#define __MO_GUI_H__

#include "Module.h"
#include <vector>

class GameObject;

class M_Gui : public Module
{
public:
	M_Gui(Application* App, bool startEnabled = true);
	~M_Gui() override;

	bool Start() override;

	void RenderCanvas2D();
	void RenderCanvas3D(); //need to think about this one

	void RenderUiElement(GameObject* uiElement);

	void CreateCanvas();
	void CreateImage();
	void CreateButton();
	void CreateCheckbox();
	void CreateText();

	void SetCanvas(int uid);
	void EraseCanvas();

public:
	std::vector<int> uid_gameobject_of_ui_selected;

private:
	int canvas = -1;
	unsigned int VAO = 0;

	unsigned int textVAO = 0;
	unsigned int textVBO = 0;
};

const float arrayUiVAO[] = {
-1, -1,
1, -1,
-1, 1,
1, -1,
1, 1,
-1, 1,
};


#endif // !__MO_GUI_H__
