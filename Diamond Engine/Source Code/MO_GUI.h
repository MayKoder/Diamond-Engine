#ifndef __MO_GUI_H__
#define __MO_GUI_H__

#include "Module.h"

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


private:
	int canvas = -1;
	unsigned int VAO = 0;
};

const float uiVAO[] = {
-1, -1,
1, -1,
-1, 1,
1, -1,
1, 1,
-1, 1,
};


#endif // !__MO_GUI_H__
