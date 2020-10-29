#pragma once

#include "Module.h"
#include<vector>

class GameObject;

class M_Scene : public Module
{

public:
	M_Scene(Application* app, bool start_enabled = true);
	virtual ~M_Scene();

	bool Init() override;
	bool Start() override;

	update_status Update(float dt) override;

	bool CleanUp() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	void Destroy(GameObject* gm);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);

	GameObject* root;

	std::vector<GameObject*> destroyList;

};