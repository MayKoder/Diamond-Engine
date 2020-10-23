#pragma once

#include "Module.h"

class M_Scene : public Module
{

public:
	M_Scene(Application* app, bool start_enabled = true);
	~M_Scene();

	bool Init() override;
	bool Start() override;

	bool CleanUp() override;
};