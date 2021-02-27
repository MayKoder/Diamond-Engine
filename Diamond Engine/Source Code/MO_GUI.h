#ifndef __MO_GUI_H__
#define __MO_GUI_H__

#include "Module.h"

class GameObject;

class M_Gui : public Module
{
public:
	M_Gui(Application* App, bool startEnabled = true);
	~M_Gui() override;

	void RenderCanvas2D();
	void RenderCanvas3D(); //need to think about this one

	void RenderUiElement(GameObject* uiElement);

	void CreateCanvas();
	void CreateImage();
	void CreateButton();

	void EraseCanvas();

public:
	int index_font;

private:

	GameObject* canvas = nullptr;
};


#endif // !__MO_GUI_H__
