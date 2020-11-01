#pragma once

#include "Module.h"

class M_FileSystem : public Module
{

public:
	M_FileSystem(Application* app, bool start_enabled = true);
	virtual ~M_FileSystem();

	bool Init() override;
	bool Start() override;

	update_status Update(float dt) override;

	bool CleanUp() override;
};