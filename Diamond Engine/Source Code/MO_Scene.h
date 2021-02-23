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

#ifndef STANDALONE
	void OnGUI() override;
#endif // !STANDALONE

	void SaveScene(const char* name);
	void LoadScene(const char* name);

	void SetGameCamera(C_Camera* cam);
	
	void CreateGameCamera(const char* name);
	void LoadModelTree(const char* modelPath);

	void CleanScene();


	GameObject* root;
	std::vector<GameObject*> destroyList;
	std::multimap<uint, SerializedField*> referenceMap;

	ResourceMaterial* defaultMaterial;

private:
	void Destroy(GameObject* gm);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);
	GameObject* LoadGOData(JSON_Object* goJsonObj, GameObject* parent);
};