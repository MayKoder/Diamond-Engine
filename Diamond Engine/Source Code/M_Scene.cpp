#include "Globals.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "Application.h"
#include "ModuleInput.h"
#include "M_Editor.h"
#include "W_Hierarchy.h"

M_Scene::M_Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
}

M_Scene::~M_Scene()
{
}

bool M_Scene::Init()
{
	root = CreateGameObject("Untitled scene", nullptr);
	return true;
}

bool M_Scene::Start()
{
	return true;
}

bool M_Scene::CleanUp()
{
	//This will delete all the gameObjects
	delete root;

	return true;
}

GameObject* M_Scene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gm = new GameObject(name);

	if(parent != nullptr)
		parent->children.push_back(gm);

	return gm;
}

void M_Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);
}

void M_Scene::RecursiveUpdate(GameObject* parent)
{
	parent->Update();

	for (size_t i = 0; i < parent->children.size(); i++)
	{
		RecursiveUpdate(parent->children[i]);
	}
}
