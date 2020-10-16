#pragma once

#include "Timer.h"
#include <vector>
#include<string>

//TODO: Yoink this
#include "Module.h"
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class M_Editor;


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	M_Editor* moduleEditor;

	std::vector<Module*> list_modules;

private:

	Timer	ms_timer;
	float	dt;
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