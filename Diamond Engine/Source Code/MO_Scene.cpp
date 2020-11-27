#include "Globals.h"
#include "GameObject.h"

#include "Application.h"

#include "MO_Input.h"
#include "MO_Editor.h"
#include "MO_Camera3D.h"
#include "MO_Scene.h"

#include "W_Inspector.h"
#include "W_Hierarchy.h"
#include"W_Game.h"

#include "IM_FileSystem.h"

#include"DEJsonSupport.h"
#include"MaykMath.h"
#include"C_Transform.h"
#include"C_Camera.h"

#include"DETime.h"

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
	FileSystem::LoadDroppedFile("Assets/skybox.fbx");
	//FileSystem::LoadFile("Assets/skybox.fbx");
	GameObject* cam = CreateGameObject("Main Camera", root);
	C_Camera* c_comp = dynamic_cast<C_Camera*>(cam->AddComponent(Component::Type::Camera));
	SetGameCamera(c_comp);

	//LoadScene("Library/Scenes/scene.json");

	return true;
}

update_status M_Scene::PreUpdate(float dt)
{
	DETime::PreUpdate();

	/*Destroy gameobjects inside the destroy queue*/
	if (destroyList.size() > 0) 
	{
		App->moduleEditor->SetSelectedGO(nullptr);
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

void M_Scene::SetGameCamera(C_Camera* cam)
{
	App->moduleRenderer3D->SetGameRenderTarget(cam);
	dynamic_cast<W_Game*>(App->moduleEditor->GetEditorWindow(EditorWindow::GAME))->SetTargetCamera(cam);
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

void M_Scene::OnGUI()
{
	if (ImGui::CollapsingHeader("Scene info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Time since game start: %f", DETime::time);
		ImGui::Text("Time scale: %f", DETime::timeScale);
		ImGui::Text("Game delta time: %f", DETime::deltaTime);
		ImGui::Text("Frame count: %i", DETime::frameCount);
		ImGui::Text("Time since engine start: %f", DETime::realTimeSinceStartup);
		ImGui::Text("Engine delta time: %f", DETime::realTimeDeltaTime);
		ImGui::Text("Engine start time: %f", DETime::realStartTime);
		ImGui::Text("Game state %s", DETime::GetStateString());
	}
}

void M_Scene::SaveScene(const char* name)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	root_object.WriteVector3("EditorCameraPosition", &App->moduleCamera->editorCamera.camFrustrum.pos.x);
	root_object.WriteVector3("EditorCameraZ", &App->moduleCamera->editorCamera.camFrustrum.front.x);
	root_object.WriteVector3("EditorCameraY", &App->moduleCamera->editorCamera.camFrustrum.up.x);
	//DEJson::WriteVector3(root_object, "EditorCameraZ", &App->moduleCamera->Z);

	JSON_Value* goArray = json_value_init_array();
	root->SaveToJson(json_value_get_array(goArray));
	json_object_set_value(root_object.nObj, "Game Objects", goArray);

	//Save file 
	std::string dir = SCENES_PATH;
	dir += name;
	json_serialize_to_file_pretty(file, dir.c_str());
	dir.clear();

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
	App->moduleEditor->SetSelectedGO(nullptr);

	SetGameCamera(nullptr);


	JSON_Object* sceneObj = json_value_get_object(scene);

	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.pos.x, &DEJson::ReadVector3(sceneObj, "EditorCameraPosition")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.front.x, &DEJson::ReadVector3(sceneObj, "EditorCameraZ")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.up.x, &DEJson::ReadVector3(sceneObj, "EditorCameraY")[0], 3);
	//MaykMath::GeneralDataSet(&App->moduleCamera->Z.x, &DEJson::ReadVector3(sceneObj, "EditorCameraZ")[0], 3);



	JSON_Array* sceneGO = json_object_get_array(sceneObj, "Game Objects");

	JSON_Object* goJsonObj = json_array_get_object(sceneGO, 0);
	root = CreateGameObject(json_object_get_string(goJsonObj, "name"), nullptr, json_object_get_number(goJsonObj, "UID"));

	GameObject* parent = root;
	for (size_t i = 1; i < json_array_get_count(sceneGO); i++)
	{
		goJsonObj = json_array_get_object(sceneGO, i);
		GameObject* originalParent = parent;

		while (parent != nullptr && json_object_get_number(goJsonObj, "ParentUID") != parent->UID)
			parent = parent->parent;

		if (parent == nullptr)
			parent = originalParent;

		parent = CreateGameObject(json_object_get_string(goJsonObj, "name"), parent, json_object_get_number(goJsonObj, "UID"));
		parent->LoadFromJson(goJsonObj);

	}

	//Free memory
	json_value_free(scene);
}
