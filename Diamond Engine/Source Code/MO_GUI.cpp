#include "MO_GUI.h"

#include "Application.h"

#include "GameObject.h"

M_Gui::M_Gui(Application* app, bool startEnabled) : Module(app, startEnabled),
	canvas(nullptr)
{
}


M_Gui::~M_Gui()
{
	canvas = nullptr;
}


bool M_Gui::SetSceneCanvas(GameObject* object)
{
	//TODO check the object has the canvas component
	canvas = object;

	return true;
}


void M_Gui::AddUIElement(GameObject* uiElement)
{
	if (canvas == nullptr)	//TODO Create a GO with a component canvas
	{

	}
	canvas->children.push_back(uiElement);
}


void M_Gui::DrawCanvas3D() {} //need to think about this one