#include "MO_GUI.h"

#include "Application.h"
#include "MO_Scene.h"


#include "GameObject.h"
#include "CO_Canvas.h"
#include "CO_Transform2D.h"

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


void M_Gui::DrawCanvas3D() 
{
} //need to think about this one


void M_Gui::CreateCanvas()
{
	if (canvas == nullptr)
	{
		canvas = new GameObject("Canvas", App->moduleScene->root);
		canvas->AddComponent(Component::TYPE::CANVAS);
	}
}


void M_Gui::CreateImage()
{
	if (canvas == nullptr)	//TODO Create a GO with a component canvas
		CreateCanvas();

	GameObject* image = new GameObject("Image", canvas);
	image->AddComponent(Component::TYPE::TRANSFORM_2D);
	image->AddComponent(Component::TYPE::MATERIAL);
}


void M_Gui::EraseCanvas()
{
	canvas = nullptr;
}