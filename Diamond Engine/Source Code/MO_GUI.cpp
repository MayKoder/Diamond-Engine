#include "MO_GUI.h"

#include "Application.h"
#include "MO_Scene.h"
#include "MO_Window.h"
#include "MO_Renderer3D.h"

#include "GameObject.h"
#include "CO_Transform2D.h"
#include "CO_Transform.h"
#include "CO_Material.h"
#include "CO_Camera.h"
#include "CO_Button.h"
#include "CO_Image2D.h"
#include "CO_Text.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include <stack>

#include "OpenGL.h"

M_Gui::M_Gui(Application* app, bool startEnabled) : Module(app, startEnabled),
	canvas(-1),
	VAO(0),
	uid_gameobject_of_ui_selected(0)
{
}


M_Gui::~M_Gui()
{
	glDeleteBuffers(1, &VAO);
	VAO = 0;

	glDeleteBuffers(1, &textVAO);
	textVAO = 0;
	glDeleteBuffers(1, &textVBO);
	textVBO = 0;

	canvas = -1;
}


bool M_Gui::Start()
{
	//Generate ui buffer
	glGenBuffers(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, arrayUiVAO, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate text buffer
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);

	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}


void M_Gui::RenderCanvas2D()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);

	if (canvasGO != nullptr)
	{
		std::stack<GameObject*> stack, stackToDraw;		//Need to traverse the tree in postorder to draw the childs in the front
		GameObject* node = nullptr;

		int elementsCount = canvasGO->children.size();
		for (int i = 0; i < elementsCount; ++i)
		{
			stack.push(canvasGO->children[i]);

			while (stack.empty() == false)
			{
				node = stack.top();
				stack.pop();
				stackToDraw.push(node);

				int childNumber = node->children.size();
				for (int i = 0; i < childNumber; ++i)
					stack.push(node->children[i]);
			}
		}

		while (stackToDraw.empty() == false)
		{
			node = stackToDraw.top();
			stackToDraw.pop();
			RenderUiElement(node);
		}
	}
}


void M_Gui::RenderCanvas3D()
{}


void M_Gui::RenderUiElement(GameObject* uiElement)
{
	Component* mat = uiElement->GetComponent(Component::TYPE::MATERIAL);
	Component* trans2D = uiElement->GetComponent(Component::TYPE::TRANSFORM_2D);
	Component* img2D = uiElement->GetComponent(Component::TYPE::IMAGE_2D);
	Component* txt = uiElement->GetComponent(Component::TYPE::TEXT_UI);

	if (mat != nullptr && trans2D != nullptr && (img2D != nullptr || txt != nullptr))
	{
		C_Transform2D* transform = static_cast<C_Transform2D*>(trans2D);
		ResourceMaterial* material = static_cast<C_Material*>(mat)->material;

		if (material->shader)
		{
			if (img2D != nullptr)
				static_cast<C_Image2D*>(img2D)->RenderImage(transform->GetGlobal2DTransform().ptr(), material, VAO);

			if (txt != nullptr)
				static_cast<C_Text*>(txt)->RenderText(transform, material, textVAO, textVBO);
		}
	}
}


void M_Gui::CreateCanvas()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		canvasGO = new GameObject("Canvas", App->moduleScene->root);
		canvasGO->AddComponent(Component::TYPE::CANVAS);
	}
}


void M_Gui::CreateImage()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* image = new GameObject("Image", canvasGO);
	image->AddComponent(Component::TYPE::TRANSFORM_2D);
	image->AddComponent(Component::TYPE::MATERIAL);
	image->AddComponent(Component::TYPE::IMAGE_2D);

}

void M_Gui::CreateButton()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* button = new GameObject("Button", canvasGO);
	button->AddComponent(Component::TYPE::TRANSFORM_2D);
	button->AddComponent(Component::TYPE::MATERIAL);
	button->AddComponent(Component::TYPE::BUTTON);
	button->AddComponent(Component::TYPE::IMAGE_2D);
	button->AddComponent(Component::TYPE::NAVIGATION, "Button");
}

void M_Gui::CreateCheckbox()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* checkbox = new GameObject("Checkbox", canvasGO);
	checkbox->AddComponent(Component::TYPE::TRANSFORM_2D);
	checkbox->AddComponent(Component::TYPE::MATERIAL);
	checkbox->AddComponent(Component::TYPE::CHECKBOX);
	checkbox->AddComponent(Component::TYPE::IMAGE_2D);
	checkbox->AddComponent(Component::TYPE::NAVIGATION, "Checkbox");
}

void M_Gui::CreateText()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* text = new GameObject("Text", canvasGO);
	text->AddComponent(Component::TYPE::TRANSFORM_2D);
	text->AddComponent(Component::TYPE::MATERIAL);
	text->AddComponent(Component::TYPE::TEXT_UI);
}


void M_Gui::SetCanvas(int uid)
{
	canvas = uid;
}


void M_Gui::EraseCanvas()
{
	canvas = -1;
}