#include "Globals.h"
#include "GameObject.h"

#include "Application.h"

#include "ModuleInput.h"
#include "M_Editor.h"
#include "ModuleCamera3D.h"
#include "M_Scene.h"

#include "W_Inspector.h"
#include "W_Hierarchy.h"

#include "I_FileSystem.h"

#include"DEJsonSupport.h"
#include"C_Transform.h"

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
	//FileSystem::LoadFile("Assets/skybox.fbx");
	return true;
}

update_status M_Scene::PreUpdate(float dt)
{
	/*Destroy gameobjects inside the destroy queue*/
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

void M_Scene::SaveScene()
{
	JSON_Value* file = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(file);

	DEJson::WriteVector3(root_object, "EditorCameraPosition", &App->moduleCamera->Position);
	DEJson::WriteVector3(root_object, "EditorCameraX", &App->moduleCamera->X);
	DEJson::WriteVector3(root_object, "EditorCameraY", &App->moduleCamera->Y);
	DEJson::WriteVector3(root_object, "EditorCameraZ", &App->moduleCamera->Z);

	JSON_Value* goArray = json_value_init_array();
	GoToJSON(root, json_value_get_array(goArray));
	json_object_set_value(root_object, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, "Library/Scenes/scene.json");

	//Free memory
	json_value_free(file);
}

void M_Scene::GoToJSON(GameObject* go, JSON_Array* jsonObj)
{
	JSON_Value* goValue = json_value_init_object();
	JSON_Object* goData = json_value_get_object(goValue);

	//Save all gameObject data
	json_object_set_string(goData, "name", go->name.c_str());

	DEJson::WriteVector3(goData, "Position", &dynamic_cast<C_Transform*>(go->GetComponent(Component::Type::Transform))->position[0]);
	DEJson::WriteVector3(goData, "Rotation", &dynamic_cast<C_Transform*>(go->GetComponent(Component::Type::Transform))->rotation.x);
	DEJson::WriteVector3(goData, "Scale", &dynamic_cast<C_Transform*>(go->GetComponent(Component::Type::Transform))->localScale[0]);

	DEJson::WriteInt(goData, "UID", go->UID);

	if(go->parent)
		DEJson::WriteInt(goData, "ParentUID", go->parent->UID);

	json_array_append_value(jsonObj, goValue);

	//TODO: Move inside component base
	{
		//Save components
		JSON_Value* goArray = json_value_init_array();
		JSON_Array* jsArray = json_value_get_array(goArray);
		for (size_t i = 0; i < go->components.size(); i++)
		{
			JSON_Value* nVal = json_value_init_object();
			JSON_Object* nObj = json_value_get_object(nVal);

			go->components[i]->SaveComponent(nObj);
			json_array_append_value(jsArray, nVal);
		}
		json_object_set_value(goData, "Components", goArray);
	}

	for (size_t i = 0; i < go->children.size(); i++)
	{
		GoToJSON(go->children[i], jsonObj);
	}
}
