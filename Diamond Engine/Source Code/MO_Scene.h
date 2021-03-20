#pragma once

#include "Module.h"
#include<vector>
#include<map>

#include"parson/parson.h"
class GameObject;
class C_Camera;
typedef unsigned int uint;
struct SerializedField;
class ResourceMaterial;
struct ActionToRealize;

class M_Scene : public Module
{

public:
	M_Scene(Application* app, bool start_enabled = true);
	virtual ~M_Scene();

	bool Init() override;
	bool Start() override;

	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;

	bool CleanUp() override;

	GameObject* GetGOFromUID(GameObject* n, uint sUID);
	GameObject* CreateGameObject(const char* name, GameObject* parent, int _uid = -1);
	void GetAllGameObjects(std::vector<GameObject*>& gameObjects);

	void LoadScriptsData(GameObject* rootObject = nullptr);
	void LoadNavigationData();

	GameObject* FindObjectWithTag(GameObject* rootGameObject, const char* tag);
	void FindGameObjectsWithTag(const char* tag, std::vector<GameObject*>& taggedObjects);

#ifndef STANDALONE
	void OnGUI() override;

	void SaveScene(const char* name);
#endif // !STANDALONE
	void LoadScene(const char* name);

	void SetGameCamera(C_Camera* cam);
	
	void CreateGameCamera(const char* name);
	void LoadModelTree(const char* modelPath);

	void CleanScene();
	GameObject* LoadGOData(JSON_Object* goJsonObj, GameObject* parent);

	int holdUID;

	GameObject* root;
	std::vector<GameObject*> destroyList;
	std::multimap<uint, SerializedField*> referenceMap;
	std::multimap<uint, ActionToRealize*> navigationReferenceMap;
	ResourceMaterial* defaultMaterial;
	char current_scene[64];
	char current_scene_name[32];

	std::vector<std::string> tags;
	std::vector<std::string> layers;

	void LoadHoldScene();
private:
	void Destroy(GameObject* gm);

	void UpdateGameObjects();

	void RecursiveUpdate(GameObject* parent);

	void RecursivePostUpdate(GameObject* parent);

	void RecursiveDontDestroy(GameObject* parent, std::vector<GameObject*>& dontDestroyList);

};