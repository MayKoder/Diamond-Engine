#pragma once

#include "Globals.h"
#include "Timer.h"
#include <vector>
#include<string>

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> list_modules;
	bool quitApplicationState;

public:
	int fpsCap;

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void ExitApplication();
	bool GetExitState();

	float GetDT() const;
	float GetFrameRate() const;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* Engine;