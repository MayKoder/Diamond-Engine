#include "Globals.h"
#include "GameObject.h"

#include "Application.h"

#include "MO_Input.h"
#include "MO_Editor.h"
#include "MO_Camera3D.h"
#include "MO_Scene.h"
#include "MO_ResourceManager.h"
#include"MO_MonoManager.h"

#include "WI_Hierarchy.h"
#include"WI_Game.h"
#include"MaykMath.h"

#include "IM_FileSystem.h"

#include"DEJsonSupport.h"
#include"CO_Transform.h"
#include"CO_Camera.h"
#include"CO_Script.h"
#include"CO_Navigation.h"

#include"RE_Texture.h"
#include"DETime.h"
#include"RE_Material.h"

#include "COMM_DeleteGO.h"

M_Scene::M_Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr),
defaultMaterial(nullptr), holdUID(0)
{
	current_scene[0] = '\0';
	current_scene_name[0] = '\0';

	tags = { "Untagged"};
	layers = { "Default" };
}

M_Scene::~M_Scene()
{
	root = nullptr;
	defaultMaterial = nullptr;

	tags.clear();
	layers.clear();
}

bool M_Scene::Init()
{
	root = CreateGameObject("Scene root", nullptr);

	return true;
}

bool M_Scene::Start()
{
	CreateGameCamera("Main Camera");
	//LoadScene("Library/Scenes/1726826608.des");
	LoadScene("Library/Scenes/1482507639.des");

	//LoadScene("Library/Scenes/884741631.des");
	//LoadScene("Library/Scenes/tmp.des");
	
#ifndef STANDALONE
	//TODO IMPORTANT: This is why we should save icons .meta, or we could generate them every time
	//But this will introduce some randomized problems with ID duplications
	// TODO: Maybe this should be handled on the editor module? texture #include is stupid
	App->moduleEditor->editorIcons.LoadPreDefinedIcons();
#endif // !STANDALONE


	return true;
}

update_status M_Scene::PreUpdate(float dt)
{
	/*Destroy gameobjects inside the destroy queue*/
	if (destroyList.size() > 0) 
	{
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
#ifndef STANDALONE

	//TODO: Only do c/v when the hier or scene widows are focused?
	if (App->moduleInput->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->moduleInput->GetKey(SDL_SCANCODE_C) == KEY_DOWN && App->moduleEditor->GetSelectedGO() != nullptr)
	{
		JSON_Value* file = json_value_init_object();
		DEConfig root_object(json_value_get_object(file));

		JSON_Value* goArray = json_value_init_array();
		App->moduleEditor->GetSelectedGO()->SaveToJson(json_value_get_array(goArray));
		json_object_set_value(root_object.nObj, "Game Objects", goArray);

		//Save file 
		json_serialize_to_file_pretty(file, "EngineIcons/cntlC.json");

		//Free memory
		json_value_free(file);
	}
	if (App->moduleInput->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->moduleInput->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		if (FileSystem::Exists("EngineIcons/cntlC.json") == true) 
		{
			JSON_Value* scene = json_parse_file("EngineIcons/cntlC.json");

			//TODO: Duplicated code from scene loading && delete command, move to method
			if (scene != NULL) 
			{
				JSON_Object* sceneObj = json_value_get_object(scene);
				JSON_Array* sceneGO = json_object_get_array(sceneObj, "Game Objects");

				GameObject* parent = (App->moduleEditor->GetSelectedGO() == nullptr) ? root : App->moduleEditor->GetSelectedGO();
				GameObject* gameObjectRoot = nullptr;
				for (size_t i = 0; i < json_array_get_count(sceneGO); i++)
				{
					parent = LoadGOData(json_array_get_object(sceneGO, i), parent);
					
					if (i == 0)
						gameObjectRoot = parent;
				}

				gameObjectRoot->RecursiveUIDRegeneration();

				LoadNavigationData();
				LoadScriptsData();

				//Free memory
				json_value_free(scene);
			}
		}
	}

	if (App->moduleInput->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->moduleInput->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		if (current_scene[0] == '\0')
		{
			std::string sceneDir = M_FileSystem::OpenSaveAsDialog();
			App->moduleFileSystem->ToLocalAssetsPath(sceneDir);
			if (!sceneDir.empty())
			{
				App->moduleScene->SaveScene(sceneDir.c_str());
				App->moduleResources->NeedsDirsUpdate(App->moduleResources->assetsRoot);
				strcpy(current_scene, sceneDir.c_str());
			
				std::string scene_name;
				FileSystem::GetFileName(sceneDir.c_str(), scene_name, false);
				strcpy(current_scene_name, scene_name.c_str());
			}
		}
		else
		{
			App->moduleScene->SaveScene(current_scene);
			App->moduleResources->NeedsDirsUpdate(App->moduleResources->assetsRoot);
		}
	}

	App->moduleEditor->shortcutManager.HandleInput();
	if (App->moduleInput->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && App->moduleEditor->GetSelectedGO() != nullptr && App->moduleEditor->GetSelectedAsset() == nullptr)
	{
		App->moduleEditor->shortcutManager.PushCommand(new COMM_DeleteGO(App->moduleEditor->GetSelectedGO()));
		App->moduleEditor->GetSelectedGO()->Destroy();
	}
#endif // !STANDALONE

	UpdateGameObjects();
	RecursivePostUpdate(root);

	return update_status::UPDATE_CONTINUE;
}

bool M_Scene::CleanUp()
{
	//This will delete all the gameObjects
	if (defaultMaterial != nullptr)
		EngineExternal->moduleResources->UnloadResource(defaultMaterial->GetUID());

	delete root;
	return true;
}

//Think about todo: Maybe avoid recursivity? Could store all GO'S in a vector and just iterate as linear?
GameObject* M_Scene::GetGOFromUID(GameObject* n, uint sUID)
{
	if (n->UID == sUID)
		return n;

	GameObject* ret = nullptr;
	for (size_t i = 0; i < n->children.size(); i++)
	{
		ret = GetGOFromUID(n->children[i], sUID);
		if (ret != nullptr)
			return ret;
	}

	return nullptr;
}

GameObject* M_Scene::CreateGameObject(const char* name, GameObject* parent, int _uid)
{
	GameObject* gm = new GameObject(name, parent, _uid);
	return gm;
}

void M_Scene::GetAllGameObjects(std::vector<GameObject*>& gameObjects)
{
	root->CollectChilds(gameObjects);
}

void M_Scene::LoadScriptsData(GameObject* rootObject)
{
	for (auto i = referenceMap.begin(); i != referenceMap.end(); ++i)
	{
		// Get the range of the current key
		auto range = referenceMap.equal_range(i->first);

		// Now render out that whole range
		for (auto d = range.first; d != range.second; ++d)
		{
			if (rootObject != nullptr)
			{
				GameObject* gameObject = GetGOFromUID(rootObject, d->first);

				if (gameObject != nullptr)
					d->second->fiValue.goValue = gameObject;

				else
					d->second->fiValue.goValue = GetGOFromUID(EngineExternal->moduleScene->root, d->first);
			}
			else
			{
				d->second->fiValue.goValue = GetGOFromUID(EngineExternal->moduleScene->root, d->first);
			}

			if (d->second->fiValue.goValue)
			{
				if (std::find(d->second->fiValue.goValue->csReferences.begin(), d->second->fiValue.goValue->csReferences.end(), d->second) == d->second->fiValue.goValue->csReferences.end())
					d->second->fiValue.goValue->csReferences.push_back(d->second);

				d->second->parentSC->SetField(d->second->field, d->second->fiValue.goValue);
			}
		}
	}

	referenceMap.clear();
}


void M_Scene::LoadNavigationData()
{
	for (auto i = navigationReferenceMap.begin(); i != navigationReferenceMap.end(); ++i)
	{
		// Get the range of the current key
		auto range = navigationReferenceMap.equal_range(i->first);
		
		GameObject* ref = GetGOFromUID(EngineExternal->moduleScene->root, i->first);
		// Now render out that whole range
		for (auto d = range.first; d != range.second; ++d)
		{
			d->second->referenceGO = ref;			
		}
	}

	navigationReferenceMap.clear();
}

GameObject* M_Scene::FindObjectWithTag(GameObject* rootGameObject, const char* tag)
{
	if (rootGameObject->CompareTag(tag))
		return rootGameObject;

	GameObject* ret = nullptr;
	for (size_t i = 0; i < rootGameObject->children.size(); i++)
	{
		ret = FindObjectWithTag(rootGameObject->children[i], tag);
		if (ret != nullptr)
			return ret;
	}

	return nullptr;
}

void M_Scene::FindGameObjectsWithTag(const char* tag, std::vector<GameObject*>& taggedObjects)
{
	std::vector<GameObject*> gameObjects;
	root->CollectChilds(gameObjects);

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->CompareTag(tag))
		{
			taggedObjects.push_back(gameObjects[i]);
		}
	}

	gameObjects.clear();
}

void M_Scene::SetGameCamera(C_Camera* cam)
{
	App->moduleRenderer3D->SetGameRenderTarget(cam);

#ifndef STANDALONE
	dynamic_cast<W_Game*>(App->moduleEditor->GetEditorWindow(EditorWindow::GAME))->SetTargetCamera(cam);
#endif // !STANDALONE
}

void M_Scene::CreateGameCamera(const char* name)
{
	GameObject* cam = CreateGameObject(name, root);
	C_Camera* c_comp = dynamic_cast<C_Camera*>(cam->AddComponent(Component::TYPE::CAMERA));

	//SetGameCamera(c_comp);
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
	gm->parent->children.shrink_to_fit();

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

	void M_Scene::LoadHoldScene()
{
	if (holdUID != 0)
	{
		std::string ret = { "Library/Scenes/" + std::to_string(holdUID) + ".des" };
		LoadScene(ret.c_str());
		holdUID = 0;
	}
}

void M_Scene::RecursivePostUpdate(GameObject* parent)
{
	if (parent->toDelete)
		return;

	if (parent->isActive())
	{
		parent->PostUpdate();

		for (size_t i = 0; i < parent->children.size(); i++)
		{
			RecursivePostUpdate(parent->children[i]);
		}
	}
}

void M_Scene::RecursiveDontDestroy(GameObject* child, std::vector<GameObject*>& dontDestroyList)
{
	if (child->dontDestroy) {
		dontDestroyList.push_back(child);
		child->parent->RemoveChild(child);
	}
	else {
		for (size_t i = 0; i < child->children.size(); ++i)
		{
			RecursiveDontDestroy(child->children[i],dontDestroyList);
		}
	}

}

#ifndef STANDALONE
void M_Scene::OnGUI()
{
	if (ImGui::CollapsingHeader("Scene info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Time since game start: %f", DETime::time);
		ImGui::SliderFloat("Time scale", &DETime::timeScale, 0.f, 3.f);
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

	//Tags ===================================================================
	JSON_Value* tagsValue = json_value_init_array();
	JSON_Array* tagsArray = json_value_get_array(tagsValue);
	for (size_t t = 0; t < tags.size(); t++)
	{
		if(strcmp(tags[t].c_str(), "Untagged") != 0)
			json_array_append_string(tagsArray, tags[t].c_str());
	}
	json_object_set_value(root_object.nObj, "tags", tagsValue);

	//Layers =================================================================
	JSON_Value* layersValue = json_value_init_array();
	JSON_Array* layersArray = json_value_get_array(layersValue);
	for (size_t l = 0; l < layers.size(); l++)
	{
		if (strcmp(layers[l].c_str(), "Default") != 0)
			json_array_append_string(layersArray, layers[l].c_str());
	}
	json_object_set_value(root_object.nObj, "layers", layersValue);

	JSON_Value* goArray = json_value_init_array();
	root->SaveToJson(json_value_get_array(goArray));
	json_object_set_value(root_object.nObj, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, name);

	//Free memory
	json_value_free(file);
	LOG(LogType::L_NORMAL, "Scene saved at: %s", name);
}
#endif // !STANDALONE

void M_Scene::LoadScene(const char* name)
{
	//TODO: ASK IF USER WANTS TO SAVE CHANGES
	JSON_Value* scene = json_parse_file(name);

	if (scene == NULL)
		return;

#ifndef STANDALONE
	App->moduleEditor->shortcutManager.DeleteCommandHistory();
	App->moduleEditor->ClearConsole();
#endif // !STANDALONE

	//TODO Save gameobjects to dont destroy list
	std::vector<GameObject*> dontDestroyList;

	if (DETime::state == GameState::PLAY) {

		RecursiveDontDestroy(root, dontDestroyList);
	}
	//Clear all current scene memory
	destroyList.clear();
	CleanScene();
	RELEASE(root); //Had to remove root to create it later

	JSON_Object* sceneObj = json_value_get_object(scene);

#ifndef STANDALONE

	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.pos.x, &DEJson::ReadVector3(sceneObj, "EditorCameraPosition")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.front.x, &DEJson::ReadVector3(sceneObj, "EditorCameraZ")[0], 3);
	MaykMath::GeneralDataSet(&App->moduleCamera->editorCamera.camFrustrum.up.x, &DEJson::ReadVector3(sceneObj, "EditorCameraY")[0], 3);
#endif // !STANDALONE

	tags.clear();
	tags = { "Untagged" };
	JSON_Array* tagsArray = json_object_get_array(sceneObj, "tags");
	for (size_t t = 0; t < json_array_get_count(tagsArray); t++)
	{
		tags.push_back(json_array_get_string(tagsArray, t));
	}

	layers.clear();
	layers = { "Default" };
	JSON_Array* layersArray = json_object_get_array(sceneObj, "layers");
	for (size_t l = 0; l < json_array_get_count(layersArray); l++)
	{
		layers.push_back(json_array_get_string(layersArray, l));
	}

	JSON_Array* sceneGO = json_object_get_array(sceneObj, "Game Objects");
	JSON_Object* goJsonObj = json_array_get_object(sceneGO, 0);
	
	root = CreateGameObject(json_object_get_string(goJsonObj, "name"), nullptr, json_object_get_number(goJsonObj, "UID"));

	GameObject* parent = root;
	for (size_t i = 1; i < json_array_get_count(sceneGO); i++)
	{
		parent = LoadGOData(json_array_get_object(sceneGO, i), parent);
	}

	LoadNavigationData();
	LoadScriptsData();

	//Insert Dont destroy objects to new scene
	for (size_t i = 0; i < dontDestroyList.size(); i++)
	{
		dontDestroyList[i]->ChangeParent(root);
	}


	//Free memory
	json_value_free(scene);
	strcpy(current_scene, name);

	std::string scene_name;
	FileSystem::GetFileName(name, scene_name, false);

	strcpy(current_scene_name, scene_name.c_str());

}

void M_Scene::LoadModelTree(const char* modelPath)
{
	JSON_Value* scene = json_parse_file(modelPath);

	if (scene == NULL)
		return;

	//Clear all current scene memory
	JSON_Object* sceneObj = json_value_get_object(scene);


	JSON_Array* sceneGO = json_object_get_array(sceneObj, "Model Objects");

	JSON_Object* goJsonObj = json_array_get_object(sceneGO, 0);
	GameObject* mRoot = CreateGameObject(json_object_get_string(goJsonObj, "name"), root, json_object_get_number(goJsonObj, "UID"));
	mRoot->LoadFromJson(goJsonObj);

	GameObject* parent = mRoot;
	for (size_t i = 1; i < json_array_get_count(sceneGO); i++)
	{
		parent = LoadGOData(json_array_get_object(sceneGO, i), parent);
	}

	//Free memory
	json_value_free(scene);

	mRoot->RecursiveUIDRegeneration();
}

void M_Scene::CleanScene()
{
	if (root == nullptr)
		return;
	delete root;
	root = nullptr;

#ifndef STANDALONE
	App->moduleEditor->SetSelectedGO(nullptr);
	SetGameCamera(nullptr);
#endif

	root = CreateGameObject("Scene root", nullptr);
}

GameObject* M_Scene::LoadGOData(JSON_Object* goJsonObj, GameObject* parent)
{
	GameObject* originalParent = parent;

	while (parent != nullptr && json_object_get_number(goJsonObj, "ParentUID") != parent->UID)
		parent = parent->parent;

	if (parent == nullptr)
		parent = originalParent;

	parent = CreateGameObject(json_object_get_string(goJsonObj, "name"), parent, json_object_get_number(goJsonObj, "UID"));
	parent->LoadFromJson(goJsonObj);
	return parent;
}