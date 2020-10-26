#pragma once

#include "Module.h"

class GameObject;

class M_Scene : public Module
{

public:
	M_Scene(Application* app, bool start_enabled = true);
	virtual ~M_Scene();

	bool Init() override;
	bool Start() override;

	//update_status Update(float dt) override;

	bool CleanUp() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);

	void UpdateGameObjects();
	void RecursiveUpdate(GameObject* parent);

	GameObject* root;


};