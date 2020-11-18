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
#include"MaykMath.h"
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

	GameObject* cam = CreateGameObject("Main Camera", root);
	cam->AddComponent(Component::Type::Camera);

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
	if (destroyList.size() > 0) 
	{
		dynamic_cast<W_Inspector*>(App->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR))->selectedGO = nullptr;
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

GameObject* M_Scene::CreateGameObject(const char* name, GameObject* parent, int _uid)
{
	GameObject* gm = new GameObject(name, parent, _uid);

	//if (parent != nullptr)
	//	parent->children.push_back(gm);

	return gm;
}

void M_Scene::Destroy(GameObject* gm)
{
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
	root->SaveToJson(json_value_get_array(goArray));
	json_object_set_value(root_object, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, "Library/Scenes/scene.json");

	//Free memory
	json_value_free(file);
}

void M_Scene::LoadScene(const char* name)
{
	//TODO: ASK IF USER WANTS TO SAVE CHANGES

	JSON_Value* scene = json_parse_file(name);

	if (scene == NULL)
		return;

	//Clear all current scene memory
	delete root;
	root = nullptr;
	dynamic_cast<W_Inspector*>(App->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR))->selectedGO = nullptr;

	JSON_Object* sceneObj = json_value_get_object(scene);

	MaykMath::GeneralDataSet(&App->moduleCamera->Position.x, &DEJson::ReadVector3(sceneObj, "EditorCameraPosition")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->X.x, &DEJson::ReadVector3(sceneObj, "EditorCameraX")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->Y.x, &DEJson::ReadVector3(sceneObj, "EditorCameraY")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->Z.x, &DEJson::ReadVector3(sceneObj, "EditorCameraZ")[0], 3);



	JSON_Array* sceneGO = json_object_get_array(sceneObj, "Game Objects");

	JSON_Object* goJsonObj = json_array_get_object(sceneGO, 0);
	root = CreateGameObject(json_object_get_string(goJsonObj, "name"), nullptr, json_object_get_number(goJsonObj, "UID"));

	GameObject* parent = root;
	for (size_t i = 1; i < json_array_get_count(sceneGO); i++)
	{
		goJsonObj = json_array_get_object(sceneGO, i);

		while (parent != nullptr && json_object_get_number(goJsonObj, "ParentUID") != parent->UID)
			parent = parent->parent;

		parent = CreateGameObject(json_object_get_string(goJsonObj, "name"), parent, json_object_get_number(goJsonObj, "UID"));
		if(parent != nullptr)
			parent->transform->SetTransformMatrix(DEJson::ReadVector3(goJsonObj, "Position"), DEJson::ReadQuat(goJsonObj, "Rotation"), DEJson::ReadVector3(goJsonObj, "Scale"));

		parent->LoadComponents(json_object_get_array(goJsonObj, "Components"));

	}

	//Free memory
	json_value_free(scene);
}
