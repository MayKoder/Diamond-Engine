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
	void LoadScriptsData();

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
	ResourceMaterial* defaultMaterial;
	char current_scene[64];
	char current_scene_name[32];


	void LoadHoldScene();
private:
	void Destroy(GameObject* gm);

	void UpdateGameObjects();

	void RecursiveUpdate(GameObject* parent);

	void RecursivePostUpdate(GameObject* parent);
};