#include "Globals.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "Application.h"
#include "ModuleInput.h"
#include "M_Editor.h"
#include "W_Hierarchy.h"
#include"W_Inspector.h"
#include "I_FileSystem.h"

M_Scene::M_Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
}

M_Scene::~M_Scene()
{
}

bool M_Scene::Init()
{
	root = CreateGameObject("Scene root", nullptr);

	return true;
}

bool M_Scene::Start()
{
	FileSystem::LoadFile("Assets/BakerHouse.fbx");
	FileSystem::LoadFile("Assets/skybox.fbx");
	return true;
}

update_status M_Scene::PreUpdate(float dt)
{

	if (destroyList.size() > 0) {
		for (size_t i = 0; i < destroyList.size(); ++i)
		{
			Destroy(destroyList[i]);
		}
		destroyList.clear();
	}
	return update_status::UPDATE_CONTINUE;
}

update_status M_Scene::Update(float dt)
{
	if (App->moduleInput->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && App->moduleEditor->GetSelectedGO() != nullptr)
		App->moduleEditor->GetSelectedGO()->Destroy();

	UpdateGameObjects();

	return update_status::UPDATE_CONTINUE;
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

void M_Scene::Destroy(GameObject* gm)
{
	dynamic_cast<W_Inspector*>(App->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR))->selectedGO = nullptr;

	for (std::vector<GameObject*>::iterator i = gm->parent->children.begin(); i != gm->parent->children.end(); ++i)
	{
		if (*i._Ptr == gm) 
		{
			gm->parent->children.erase(i);
			break;
		}
	}

	delete gm;
	gm = nullptr;
}

void M_Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);
}

void M_Scene::RecursiveUpdate(GameObject* parent)
{
	if (parent->toDelete)
	{
		destroyList.push_back(parent);
		return;
	}

	if (parent->isActive()) 
	{
		parent->Update();

		for (size_t i = 0; i < parent->children.size(); i++)
		{
			RecursiveUpdate(parent->children[i]);
		}
	}
}
