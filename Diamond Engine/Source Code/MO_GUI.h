#ifndef __MO_GUI_H__
#define __MO_GUI_H__

#include "Module.h"

class GameObject;

class M_Gui : public Module
{
public:
	M_Gui(Application* App, bool startEnabled = true);
	~M_Gui() override;

	bool SetSceneCanvas(GameObject* object);
	void DrawCanvas3D(); //need to think about this one

	void CreateCanvas();
	void CreateImage();

	void EraseCanvas();

private:

	GameObject* canvas = nullptr;
};


#endif // !__MO_GUI_H__
