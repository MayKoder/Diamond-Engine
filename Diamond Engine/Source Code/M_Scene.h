#pragma once

#include "Module.h"
#include<vector>

#include"parson/parson.h"
class GameObject;

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

	GameObject* CreateGameObject(const char* name, GameObject* parent, int _uid = -1);

	void SaveScene();
	void LoadScene(const char* name);

	GameObject* root;

	std::vector<GameObject*> destroyList;

private:
	void Destroy(GameObject* gm);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);


	//ASK: Thinking about using _lowerCase() for some private methods as a "dont use this" warning
	void GoToJSON(GameObject* go, JSON_Array* jsonObj);
};