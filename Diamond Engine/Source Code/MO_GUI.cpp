#include "MO_GUI.h"

#include "Application.h"
#include "MO_Scene.h"
#include "MO_Window.h"
#include "MO_Renderer3D.h"

#include "GameObject.h"
#include "CO_Canvas.h"
#include "CO_Transform2D.h"
#include "CO_Material.h"
#include "CO_Camera.h"
#include "CO_Button.h"

#include <stack>

#include "OpenGL.h"

M_Gui::M_Gui(Application* app, bool startEnabled) : Module(app, startEnabled),
	canvas(nullptr), index_font(-1)
{
}


M_Gui::~M_Gui()
{
	canvas = nullptr;
}


void M_Gui::RenderCanvas2D()
{
	std::stack<GameObject*> stack;
	GameObject* node = nullptr;
	
	int elementsCount = canvas->children.size();
	for (int i = 0; i < elementsCount; ++i)
	{
		stack.push(canvas->children[i]);

		while (stack.empty() == false)
		{
			node = stack.top();
			stack.pop();

			RenderUiElement(node);

			int childNumber = node->children.size();
			for (int i = 0; i < childNumber; ++i)
				stack.push(node->children[i]);
		}
	}
}


void M_Gui::RenderCanvas3D() 
{

}


void M_Gui::RenderUiElement(GameObject* uiElement)
{
	
}


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

void M_Gui::CreateButton()
{
	if (canvas == nullptr)	//TODO Create a GO with a component canvas
		CreateCanvas();

	GameObject* image = new GameObject("Button", canvas);
	image->AddComponent(Component::TYPE::TRANSFORM_2D);
	image->AddComponent(Component::TYPE::MATERIAL);
	image->AddComponent(Component::TYPE::BUTTON);
}


void M_Gui::EraseCanvas()
{
	canvas = nullptr;
}